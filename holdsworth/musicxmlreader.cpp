/* vim: set ts=8 sts=4 sw=4 expandtab: */

#include <holdsworth/musicxmlreader.h>
#include <holdsworth/instrumentdefn.h>
#include <QDebug>


namespace Holdsworth {

MusicXMLReader::MusicXMLReader(NoteList& nl)
    : notelist_(nl)
    , forced_(false)
    , restart_hint_(false)
    , notemap_()
    , resolution_(960)
    , key_sig_(0)
    , offset_(0)
{
    notemap_["C"] = 36;
    notemap_["D"] = 38;
    notemap_["E"] = 40;
    notemap_["F"] = 41;
    notemap_["G"] = 43;
    notemap_["A"] = 45;
    notemap_["B"] = 47;
}

void
MusicXMLReader::setForced(bool force)
{
    forced_ = force;
}

void
MusicXMLReader::setOffset(int offset)
{
    offset_ = offset;
}

bool
MusicXMLReader::isFlatKey() const
{
    return (key_sig_ < 0);
}

int
MusicXMLReader::keySig() const
{
    return key_sig_;
}

bool MusicXMLReader::read(QIODevice *device)
{
    setDevice(device);

    while (!atEnd()) {
	readNext();

	if (isStartElement()) {
	    if (name() == "score-partwise" ) {
		readScore();
            }
	    else {
		raiseError(QObject::tr("The file is not a MusicXML file?!"));
            }
	}
    }

    return !error();
}

void MusicXMLReader::readUnknownElement()
{
    //Q_ASSERT(isStartElement());

    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
	    readUnknownElement();
        }
    }
}

void MusicXMLReader::readScore()
{
    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
	    if (name() == "part") {
		readPart();
            }
	    else {
		readUnknownElement();
            }
	}
    }
}

void MusicXMLReader::readPart()
{
    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
	    if (name() == "measure") {
		readMeasure();
            }
	    else {
		readUnknownElement();
            }
	}
    }
}

void MusicXMLReader::readMeasure()
{
    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
	    if (name() == "note") {
		readNote();
            }
            else if (name() == "attributes") {
		readAttributes();
            }
	    else {
		readUnknownElement();
            }
	}
    }
}

void MusicXMLReader::readAttributes()
{
    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
            if (name() == "divisions") {
                readNext();
                resolution_ = text().toString().toUInt();
                qDebug() << "File resolution: " << resolution_ << " per crotchet";
		readUnknownElement();
            }
            else if (name() == "key") {
                readKey();
            }
	    else {
		readUnknownElement();
            }
	}
    }
}

void MusicXMLReader::readKey()
{
    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
            if (name() == "fifths") {
                readNext();
                key_sig_  = text().toString().toInt();
                qDebug() << "File key sig : " << key_sig_;
		readUnknownElement();
            }
	    else {
		readUnknownElement();
            }
	}
    }
}

void MusicXMLReader::readNote()
{
    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
	    if (name() == "notations") {
		readNotations();
            }
            else if (name() == "pitch") {
                readPitch();
            }
            else if (name() == "rest") {
                notelist_.push_back(Note(0));
                restart_hint_ = true;
		readUnknownElement();
            }
            else if (name() == "duration") {
                readNext();
                uint duration = text().toString().toUInt();
                duration = ((960 * duration) + 480) / resolution_;
qDebug() << "duration = " << duration;
                notelist_.back().setDuration(duration);
		readUnknownElement();
            }
	    else {
		readUnknownElement();
            }
	}
    }
}

void MusicXMLReader::readPitch()
{
    QString notename;
    uint octave = 0;
    int tweak = 0;

    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
	    if (name() == "step") {
                readNext();
                notename = text().toString();
		readUnknownElement();
            }
            else if (name() == "octave") {
                readNext();
                octave = text().toString().toUInt() - 3;
		readUnknownElement();
            }
            else if (name() == "alter") {
                readNext();
                tweak = text().toString().toInt();
		readUnknownElement();
            }
	    else {
		readUnknownElement();
            }
	}
    }
    notelist_.push_back(Note(notemap_[notename] + (12 * octave) + tweak + offset_));
    if (restart_hint_) {
        restart_hint_ = false;
        notelist_.back().addAnnotation(HINT_RESTART);
    }
}

void MusicXMLReader::readNotations()
{
    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
	    if (name() == "technical") {
		readTechnical();
            }
	    else {
		readUnknownElement();
            }
	}
    }
}

void MusicXMLReader::readTechnical()
{
    Holdsworth::InstrumentDefn t_defn; //<VN> bleh
    uint fretnum = 0;
    uint stringnum = 0;

    while (!atEnd()) {
	readNext();

	if (isEndElement()) {
	    break;
        }

	if (isStartElement()) {
	    if (name() == "fret") {
                readNext();
                fretnum = text().toString().toUInt();
		readUnknownElement();
            }
            else if (name() == "string") {
                readNext();
                /*
                 * Convert MusicXML string number to Holdsworth string number
                 */
                stringnum = 7 - (text().toString().toUInt());
                NoteNum n = t_defn.noteAt(FretPos(stringnum, fretnum)).noteNum();
                if (notelist_.back().noteNum() != n) {
                    if (n == notelist_.back().noteNum() + 12) {
                        qDebug() << "Octave Offset detected - consider using --note-offset=12 with this MusicXML source.";
                    }
                    else {
                        qDebug() << "AAAGH f" << fretnum << " s " << stringnum << "=" << n << "when notes say" << notelist_.back().noteNum();
                    }
                    notelist_.back().setNoteNum(n);
                }
                if (forced_ && (fretnum != 0)) {
                    notelist_.back().setString(stringnum);
                }
		readUnknownElement();
            }
	    else {
		readUnknownElement();
            }
	}
    }
}


}
/*
 * End
 */
