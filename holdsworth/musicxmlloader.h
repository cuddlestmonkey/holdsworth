/* vim: set ts=8 sts=4 sw=4 expandtab: */

#ifndef HoldsworthMusicXMLLoader_h
#define HoldsworthMusicXMLLoader_h

#include <QString>
#include <holdsworth/note.h>

namespace Holdsworth {

void loadMusicXML(QString filename, NoteList& nl, bool force, int& key_sig, int offset);


}

#endif

/*
 * end
 */
