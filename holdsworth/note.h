/* vim: set ts=8 sts=4 sw=4 expandtab: */
/***************************************************************************
 *   Copyright (C) 2006 by Vince Negri                                     *
 *   vince.negri@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef HOLDSWORTH_NOTE_H
#define HOLDSWORTH_NOTE_H


#include <holdsworth/types.h>
#include <string>
#include <list>

namespace Holdsworth {


/*! \brief A single note (with or without fingering data.)
 *
 * The library consumer can pass in note data with explicit fingering data.
 * Such data is always honoured by the engine; i.e. it overrides any auto-computed
 * data and will affect fingering of later notes.
 */
class Note
{
public:
    /*! Default constructor (mainly here to keep templates happy) */
    Note();

    /*! A note of indeterminate length. Used for technical fingerings. */
    explicit Note(NoteNum the_note_num);

    /*!< Normal constructor for a note.*/
    Note(NoteNum the_note_num, int the_duration, int the_time);

    virtual ~Note() {/*empty*/}

    std::string dbgDump() const;

    int duration() const {return duration_;}

    NoteNum noteNum() const {return note_num_;}         /*!< Gettor function for MIDI note number */
    StringNum stringNum() const {return f_.strg;}       /*!< Gettor function for string number */
    FingerNum fingerNum() const {return f_.finger;}     /*!< Gettor function for fingering */
    FretNum fretNum() const {return f_.fret;}           /*!< Gettor function for fret */
    Fingering fingering() const {return f_;}            /*!< Gettor function for whole fingering struct */
    Annotation annotation() const {return f_.annotation;}  	/*!< Gettor function for annotation */
    std::string annotationAsStr() const;  			/*!< Gettor function for annotation */
    std::string lilypondDiagram() const {return fretdiagram_;}

    void setFinger(FingerNum x) {f_.finger = x;}        /*!< Settor function for fingering */
    void setFret(FretNum x) {f_.fret = x;}              /*!< Settor function for fret number */
    void setString(StringNum x) {f_.strg = x;}          /*!< Settor function for string */
    void setFingering(const Fingering& x) {f_ = x;}     /*!< Quick settor function for fq fingering */
    void setNoteNum (NoteNum n) {note_num_ = n;}        /*!< Settor function for MIDI note number */

    bool isRest() const {return note_num_ == 0;}        /*!< Is this note a rest? */

    bool hasAnnotation(const Annotation x) const {return ((f_.annotation & x) != 0);}
    /*! \brief Add an annotation mark to the note.
     */
    void addAnnotation(const Annotation x) {f_.annotation = (Annotation) (f_.annotation | x);}

    /*! \brief Set duration in MusicXML units (Assuming 960 = crotchet)
     */
    void setDuration(int d) {duration_ = d;}

    /*! \brief Associate a Lilypond fretboard diagram with this note position.
     */
    void setDiagram(const std::string& x) {fretdiagram_ = x;}

    /*! \brief Remove any autohints added to the note.
     *
     * \todo this has a bug: If there are any autohints, <b>all</b> hints are removed.
     */
    void purgeAutoHints();

    /*! \brief Does this note have position shift up/down hints?
     */
    bool hasShiftHint() const;

    /*! \brief Does this note have position break (up/down/free) hints?
     */
    bool hasBreakHint() const;

    /*! \brief Does this note have position break (free) hints?
     */
    bool hasRestartHint() const;

    /*! \brief Does this note have an enforced glissando?
     */
    bool hasGlissHint() const;

    static Annotation annotationFromStr(const std::string& a);
private:
    NoteNum note_num_;
    int duration_;
    int time_;

    Fingering f_;
    std::string fretdiagram_;
};

/*! \brief An ordered list of Notes.
 */
typedef std::list<Note> NoteList;

/*! \brief An iterator into a list of Notes. (Convenience typedef)
 */
typedef NoteList::const_iterator ConstNoteIterator;

}
#endif /* HOLDSWORTH_NOTE_H */
