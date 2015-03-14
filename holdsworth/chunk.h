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

#ifndef HOLDSWORTH_CHUNK_H
#define HOLDSWORTH_CHUNK_H

#include <holdsworth/types.h>
#include <holdsworth/note.h>
#include <string>
#include <list>

namespace Holdsworth {


/*!
 *  \brief A block of fingered notes in a given position.
 *
 */
class Chunk
{
public:
    Chunk();
    virtual ~Chunk() {/*empty*/}

    unsigned int length() const;	/*!< \brief Returns number of notes in chunk */
    int cost() const;			/*!< \brief Returns overall "penalty score" of chunk. */

    void addCost(int);			/*!< \brief Add penalty points to the chunk.*/
    void addNote(const Note&);		/*!< \brief Add a note to the chunk. */

    void setPosition(FingerPosition x) {position_ = x;}	    /*!< \brief Settor function for position. */
    FingerPosition getPosition() const {return position_;}  /*!< \brief Gettor function for position. */

    void dbgDump() const;

    /*! \brief Generate fret diagram suitable for Lilypond markup.
     *
     * This will produce a fret diagram showing all notes in the chunk
     * for which the note is fingered with the "correct" finger - i.e.
     * Q-shifts are ignored.
     */
    std::string lilypondFretDiagram() const;

    /*! \brief Return a reference to the internal note list.
     *
     * This reference of course may become invalid, so it should be
     * used immediately as the RHS of a NoteList += operation.
     */
    const NoteList& noteList() const {return notes_;}

    /*! \brief Create and store a fretboard diagram for this chunk.
     */
    void makeFretDiag();

    /*! \brief Tag this chunk as a shift of LH position.
     */
    void tagPositionShift();

private:
    NoteList notes_;
    FingerPosition position_;
    int cost_;

};

/*! \brief A list of chunks.
 *
 * Typically, each chunk will be in a different position
 *  (otherwise they would be merged into a single chunk.)
 */
typedef std::list<Chunk> ChunkList;

}
#endif /* HOLDSWORTH_CHUNK_H */
