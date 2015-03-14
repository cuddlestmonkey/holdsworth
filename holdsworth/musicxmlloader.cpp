/* vim: set ts=8 sts=4 sw=4 expandtab: */

#include <holdsworth/musicxmlloader.h>
#include <holdsworth/musicxmlreader.h>
#include <QFile>
#include <QDebug>

namespace Holdsworth {

void loadMusicXML(QString filename, NoteList& nl, bool force, int& key_sig, int offset)
{
    QFile file(filename);
    if ( file.open( QIODevice::ReadOnly) ) {


        MusicXMLReader reader(nl);
        reader.setForced(force);
        reader.setOffset(offset);

        if (!reader.read(&file)) {
            qDebug("Badly-formed input XML");
        }
        else {
            key_sig = reader.keySig();
        }

        file.close();
    }
    else {
        qDebug() << "Can't open" << filename;
    }
}

}


/*
 * End
 */
