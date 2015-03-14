/* vim: set cindent ts=8 sts=4 sw=4 expandtab: */
/*
 * FING: Used for testing Holdsworth library, now also a useful program in its own right.
 */

#include <iostream>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <holdsworth/types.h>
#include <holdsworth/instrumentdefn.h>
#include <holdsworth/engine.h>
#include <holdsworth/handmodelx.h>
#include <holdsworth/handmodelx2.h>
#include <holdsworth/vn_algorithm.h>
#include <holdsworth/debugging.h>
#include <holdsworth/musicxmlloader.h>
#include "mygetopt.h"
#include "version.i"


bool quiet = false;

static void
show_usage()
{
    std::cout << "Fing " << SYS_version + 4 << std::endl;
    std::cout << "usage: fing <options>" << std::endl << std::endl;;
    std::cout << "Input Options:" << std::endl;
    std::cout << "--migt-scale=N --migt-step=M" 
        << std::endl << "--migt-start=S --migt-range=R" 
	<< std::endl << "    Generate MIGT exercise for scale number N with step M" 
	<< std::endl << "    Start on MIDI note S and range over R octaves" 
	<< std::endl << "    N can be a number or a twelve-character pattern" << std::endl;
    std::cout << "--input=FILE        Take input notes from FILE (default: inputnotes)" << std::endl;
    std::cout << "  --dumbtab         Interpret input notes as being in dumbtab format" << std::endl;
    std::cout << "  --midicsv         Interpret input notes as being in midicsv format" << std::endl;
    std::cout << "  --musicxml        Interpret input notes as being in musicXML format" << std::endl;
    std::cout << "    --note-offset=N Shift MusicXML notes by N semitones. Used if input file as notes as sounded, not as written." << std::endl;
    std::cout << "    --force           Force use of note allocations in MusicXML input" << std::endl << std::endl;
    std::cout << "Output Options:" << std::endl;
    std::cout << "--output=FILE       Output to FILE (default: input filename + .ly, or fingout for selftests)" << std::endl;
    std::cout << "  --eps             Generate lilypond suitable for generating an EPS file" << std::endl;
    std::cout << "  --use-flats       Use flats instead of sharps for accidentals" << std::endl;
    std::cout << "  --no-annotations  Don't include MIGT annotations in lilypond output" << std::endl << std::endl;
    std::cout << "Algorithm Control Options:" << std::endl;
    std::cout << "--extended          Use extended fingering" << std::endl;
    std::cout << "--extended2         Use double extended fingering" << std::endl;
    //std::cout << "--no-back-to-back   Inhibit back-to-back gliss shifts" << std::endl;
    std::cout << "--back-to-back      Allow back-to-back gliss shifts" << std::endl;
    std::cout << "--maxshift=N        Try to keep shifts to <=N frets" << std::endl << std::endl;
    std::cout << "Misc Options:" << std::endl;
    std::cout << "--quiet             Don't print cryptic progress stuff" << std::endl;
    std::cout << "--statistics        Print end-of-run statistics" << std::endl;
    std::cout << "--help              This message" << std::endl;
    //std::cout << "--test=TESTNAME     Run internal test [scale|sheets|unmerry]" << std::endl;
}

int main(int argc, char ** argv)
{
    GetOpt opts(argc, argv);

    bool usage = false;
    bool extended = false;
    bool extended2 = false;
    bool stats = false;
    bool dummy = false;
    bool allow_back_to_back_gliss = false;
    bool midicsv = false;
    bool dumbtab = false;
    bool musicxml = false;
    bool eps = false;
    bool force = false;
    bool use_flats = false;
    bool no_annotations = false;
    uint max_num_passes = 50;
    int key_sig = 0;
    int note_offset = 0;
    QString hinttxt;
    QString outfilename;
    QString infilename;
    QString testname;
    QString maxshift;
    QString max_num_passes_str;

    uint migt_scale;
    uint migt_step;
    QString migt_scale_str;
    QString migt_step_str;
    uint migt_start;
    uint migt_range;
    QString migt_start_str;
    QString migt_range_str;
    QString note_offset_str;

    opts.addSwitch("help", &usage);
    opts.addSwitch("extended", &extended);
    opts.addSwitch("extended2", &extended2);
    opts.addSwitch("statistics", &stats);
    opts.addSwitch("quiet", &quiet);
    opts.addSwitch("no-back-to-back", &dummy); // to stop old scripts breaking
    opts.addSwitch("back-to-back", &allow_back_to_back_gliss);
    opts.addSwitch("midicsv", &midicsv);
    opts.addSwitch("musicxml", &musicxml);
    opts.addSwitch("force", &force);
    opts.addSwitch("dumbtab", &dumbtab);
    opts.addSwitch("eps", &eps);
    opts.addSwitch("use-flats", &use_flats);
    opts.addSwitch("no-annotations", &no_annotations);
    opts.addOption('t', "hint", &hinttxt);
    opts.addOption('m', "maxshift", &maxshift);
    opts.addOption('p', "max-passes", &max_num_passes_str);
    opts.addOption('O', "note-offset", &note_offset_str);
    opts.addOptionalOption("output", &outfilename, "fingout");
    opts.addOptionalOption("input", &infilename, "inputnotes");
    opts.addOptionalOption("test", &testname, "unmerry");
    opts.addOptionalOption("migt-scale", &migt_scale_str, "");
    opts.addOptionalOption("migt-step", &migt_step_str, "1");
    opts.addOptionalOption("migt-start", &migt_start_str, "45");
    opts.addOptionalOption("migt-range", &migt_range_str, "2");

    if (!opts.parse()) {
        show_usage();
        return 1;
    }

    if ((argc == 1) || usage) {
        show_usage();
        return 0;
    }

    if (!infilename.isEmpty() && outfilename.isEmpty()) {
        outfilename = infilename + ".ly";
    }
        
    if (!max_num_passes_str.isEmpty()) {
        max_num_passes = max_num_passes_str.toUInt();
    }

    if (!note_offset_str.isEmpty()) {
        note_offset = note_offset_str.toInt();
    }

    Holdsworth::InstrumentDefn t_defn;
    Holdsworth::HandModel *t_model;
    if (extended2) {
	t_model = new Holdsworth::HandModelX2;
    }
    else if (extended) {
        t_model = new Holdsworth::HandModelX;
    }
    else {
        t_model = new Holdsworth::HandModel;
    }

    Holdsworth::VNAlgorithm t_alg;
    t_alg.setHandModel(t_model);
    
    Holdsworth::Constraints t_constraints;
    t_constraints.setBTBGliss(allow_back_to_back_gliss);

    Holdsworth::Engine t_engine;
    if (!maxshift.isEmpty()) {
        t_engine.setMaxLHShift(maxshift.toInt());
    }
    
    t_engine.setInstrument(&t_defn);
    t_engine.setAlgorithm(&t_alg);
    t_engine.setConstraints(&t_constraints);



    Holdsworth::NoteList nl;

    if (!migt_scale_str.isEmpty()) {

        if (migt_scale_str.length() == 12) {
            migt_scale = 0;
            for (uint i = 0; i < 12; ++i) {
                if (migt_scale_str[i] == 'O') {
                    migt_scale += (1 << i);
                }
            }
        }
        else {
            migt_scale = migt_scale_str.toUInt();
        }
	migt_step = migt_step_str.toUInt();
        if (migt_step == 0) {
            //migt_step = 1;
        }
        migt_start = migt_start_str.toUInt();
        if (migt_start == 0) {
            migt_start = 45;
        }
        migt_range = migt_range_str.toUInt() * 12;
        qDebug("migt_range_str = %s %u", migt_range_str.toLatin1().data(), migt_range);
        if (migt_range == 0) {
            migt_range = 24;
        }


	bool scale_notes[12];

	std::cout << "MIGT Scale: ";
	for (uint i = 0; i < 12; ++i) {
	    scale_notes[i] = ((migt_scale & (1 << i)) != 0);
	    if (scale_notes[i]) {
		std::cout << "O";
	    }
	    else {
		std::cout << ".";
	    }
	}
	std::cout << std::endl;

	uint scale_steps[12];
	uint num_steps = 0;

	scale_steps[num_steps] = 1;
	for (uint i = 1; i < 12; ++i) {
	    if (scale_notes[i]) {
		std::cout << scale_steps[num_steps] << "/";
		++num_steps;
		scale_steps[num_steps] = 1;
	    }
	    else {
		++scale_steps[num_steps];
	    }
	}
	std::cout << scale_steps[num_steps] << std::endl;
	if (migt_step == 0) {
	    migt_step = num_steps;
	}
	++num_steps;
	std::cout << num_steps << " note scale." << std::endl;


        nl.push_front(Holdsworth::Note(migt_start));

        for (uint k = 1; k <= migt_step; ++k) {
            uint start_note = 0;
            uint this_note = start_note;
            uint this_step = 0;

            Holdsworth::NoteList nl1;
            Holdsworth::NoteList nl2;
            do {
                //std::cout << "Add notes: " << this_note;
                nl1.push_back(Holdsworth::Note(this_note + migt_start));
                nl2.push_front(Holdsworth::Note(this_note + migt_start));
                for (uint j = 0; j < k; ++j) {
                    this_note += scale_steps[this_step];
                    this_note %= migt_range;
                    ++this_step;
                    this_step %= num_steps;
                }
            } while (this_note != start_note);

            nl1.pop_front();
            nl2.pop_front();
            nl2.push_front(Holdsworth::Note(migt_start + migt_range));

            nl.insert(nl.end(), nl1.begin(), nl1.end());
            nl.insert(nl.end(), nl2.begin(), nl2.end());
        }

    }
    else if (infilename.isEmpty()) {

#define ADD_NOTE(x, y) \
        nl.push_back(t_defn.noteAt(Holdsworth::FretPos((x), (y))))
        if (testname == "scale") {
            /*
             * At the moment the available algorithms do not take timing into account;
             * they produce "technical" fingerings.
             * 
             * Let's try a major scale
             */

            nl.push_back(Holdsworth::Note(44));
            nl.push_back(Holdsworth::Note(46));
            nl.push_back(Holdsworth::Note(48));
            nl.push_back(Holdsworth::Note(49));
            nl.push_back(Holdsworth::Note(51));
            nl.push_back(Holdsworth::Note(53));
            nl.push_back(Holdsworth::Note(55));
            nl.push_back(Holdsworth::Note(56));
            nl.push_back(Holdsworth::Note(58));
            nl.push_back(Holdsworth::Note(60));
            nl.push_back(Holdsworth::Note(61));
            nl.push_back(Holdsworth::Note(63));
            nl.push_back(Holdsworth::Note(65));
            nl.push_back(Holdsworth::Note(67));
            nl.push_back(Holdsworth::Note(68));
        }
        else if (testname == "sheets") {
#include "sheets-test.i"
        }
        else {
#include "unmerry-test.i"
        }
    }
    else if (musicxml) {
        Holdsworth::loadMusicXML(infilename, nl, force, key_sig, note_offset );
        if (key_sig < 0) {
            use_flats = true;
        }
    }
    else {
        QFile file(infilename);
        if ( file.open( QIODevice::ReadOnly) ) {
            QTextStream stream( &file );
            QString line;
            while ( !stream.atEnd() ) {
		QString whole_line;

                int i;
                QString note_annotation;
                QString note_type;

		if (midicsv) {
		    whole_line = stream.readLine();
		    QStringList sl = whole_line.split(',');
		    // For now..
		    if (sl.count() < 6) {
			    note_type = "x";
		    }
		    else if ((sl[2] == " Note_on_c") && (sl[5] != " 0")) {
			    note_type = "M";
			    i = sl[4].toInt();
		    }
		    else {
			    note_type = "x";
		    }
		    note_annotation = ".";
		}
		else if (dumbtab) {
		    stream >> note_type;
		    stream >> i;
		    stream >> note_annotation;
		}
		else {
		    stream >> note_type;
		    stream >> i;
		    stream >> note_annotation;
		}

                stream.skipWhiteSpace();
		if (note_type == "x") {
			;
		}
		else if (note_type == "M") {
		    qDebug() << "read Midi note" <<  i << note_annotation;
		    nl.push_back(Holdsworth::Note(i));
		}
		else {
		    if (!quiet) {
			qDebug() << "read note" << note_type << i << note_annotation;
		    }
		    ADD_NOTE(note_type.toUInt(), i);
		}

		if (dumbtab) {
		    uint stn = note_type.toUInt();
		    nl.back().setString(stn);
		    		    
		}

		if (note_annotation.startsWith("1")) {
		    nl.back().setFinger(Holdsworth::FirstFinger);
		    note_annotation.remove("1");
		}
		else if (note_annotation.startsWith("2")) {
		    nl.back().setFinger(Holdsworth::MiddleFinger);
		    note_annotation.remove("2");
		}
		else if (note_annotation.startsWith("3")) {
		    nl.back().setFinger(Holdsworth::RingFinger);
		    note_annotation.remove("3");
		}
		else if (note_annotation.startsWith("4")) {
		    nl.back().setFinger(Holdsworth::FourthFinger);
		    note_annotation.remove("4");
		}

                if (note_annotation != ".") {
                    nl.back().addAnnotation(
			    Holdsworth::Note::annotationFromStr(
				std::string(note_annotation.toLatin1().data()))); 
                }
            }
            file.close();
        }
    }

    /*
     * At the moment you have to add this as a sentinel - ugly.
     */
    nl.push_back(Holdsworth::Note(Holdsworth::NotDefined));



    //for (int p = 1; p < 10; ++p) {
        int p = max_num_passes;
        QTime t;
        t.start();
        t_engine.compute(nl, p);
        int time_taken = t.elapsed();
        if (stats) {
            std::cout << nl.size() << " notes rendered in " << time_taken << "ms. (";
            std::cout << (nl.size() * 1000) / time_taken << " notes/sec)" << std::endl;
        }

        if (outfilename.isEmpty()) {
            return 0;
        }
        else {
            if (!outfilename.endsWith(".ly")) {
                outfilename += ".ly";
            }
        }

        //outfilename += QString::number(p);
            
        QFile outfile(outfilename);
        if (outfile.open( QIODevice::WriteOnly)) {
            QTextStream outstream(&outfile);
            outstream << "\\include \"english.ly\"" << endl;
	    if (eps) {
		outstream << "\\paper{" << endl;
		outstream << "    indent=0\\mm" << endl;
		outstream << "    line-width=180\\mm" << endl;
		outstream << "    oddFooterMarkup=##f" << endl;
		outstream << "    oddHeaderMarkup=##f" << endl;
		outstream << "    bookTitleMarkup = ##f" << endl;
		outstream << "    scoreTitleMarkup = ##f" << endl;
		outstream << "}" << endl;

	    }
	    else {
		outstream << "\\header{" << endl;
		outstream << "  title = \"" << outfilename << "\"" << endl;
		outstream << "  tagline = \\markup \\center-column {\"Tab generated by the Holdsworth library\" \"Command Line: fing " << opts.cmdline() << "\"}" << endl;
		outstream <<    "}" << endl;
	    }

            outstream << "frag = {" << endl;
            t_engine.dumpLilyPond(outstream, use_flats, !no_annotations);
            outstream << "}" << endl;
            outstream << "fragt = {" << endl;
            t_engine.dumpLilyPondTab(outstream);
            outstream << "}" << endl;
            outstream << "\\new StaffGroup << \\new Staff { \\clef \"G_8\" " 
                << Holdsworth::dbgLilypondKeySig(key_sig)
                << "  \\frag } \\new TabStaff { \\fragt } >> " << endl;
            
            outfile.close();
        }
    //}

    return 0;
}
