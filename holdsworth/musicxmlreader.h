/* vim: set ts=8 sts=4 sw=4 expandtab: */
#ifndef HoldsworthMusicXMLReader_h
#define HoldsworthMusicXMLReader_h

#include <QXmlStreamReader>
#include <QMap>
#include <holdsworth/note.h>


namespace Holdsworth {

 class MusicXMLReader : public ::QXmlStreamReader
 {
 public:
     MusicXMLReader(NoteList& nl);

     bool read(::QIODevice *device);

     void setForced(bool force);
     void setOffset(int offset);

     bool isFlatKey() const;
     int keySig() const;

 private:
     void readUnknownElement();
     void readScore();
     void readPart();
     void readMeasure();
     void readNote();
     void readNotations();
     void readTechnical();
     void readPitch();
     void readAttributes();
     void readKey();

     NoteList& notelist_;
     bool forced_;
     bool restart_hint_;
     QMap<QString,uint> notemap_;
     uint resolution_;
     int key_sig_;
     int offset_;
};

}

#endif
