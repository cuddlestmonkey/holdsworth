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

#ifndef HOLDSWORTH_ALGORITHM_H
#define HOLDSWORTH_ALGORITHM_H


#include <holdsworth/types.h>
#include <holdsworth/note.h>
#include <holdsworth/chunk.h>

namespace Holdsworth {

class InstrumentDefn;
class Constraints;
class HandModel;

/*!
 * \brief Base class for an algorithm that plots out feasible fingerings.
 *
 * The algorithm is constrained by an associated InstrumentDefn and some
 * Constraints.
 */
class Algorithm
{
public:
    Algorithm();
    virtual ~Algorithm() {/*empty*/}

    /*!
     * \brief Build a block of fingering.
     *
     * The algorithm is expected to return the largest feasible chunk
     * of fingerings possible without abandoning position.
     */
    virtual Chunk generateChunk(ConstNoteIterator starting_note, const FingerPosition&, const FretPos&, const Fingering& force_first, const Note *lead_in) = 0;

    /*! \brief Return candidate positions for a given position on the
     * fingerboard.
     */
    virtual FingerPositionList candidates(const FretPos&);

    /*! \brief Return candidate fret positions for a given note.
     *
     * This is a convenience wrapper which calls into the underlying method of the
     * InstrumentDefn.
     */
    virtual FretPosList candidates(const Note&);


    void setInstrument(InstrumentDefn *);   /*!< \brief Settor function for associated InstrumentDefn */
    void setConstraints(Constraints *);	    /*!< \brief Settor function for associated Constraints */
    void setHandModel(HandModel *);	    /*!< \brief Settor function for associated Hand Model */

protected:
    InstrumentDefn *instrument_;
    Constraints *constraints_;
    HandModel *handmodel_;

};

}
#endif /* HOLDSWORTH_ALGORITHM_H */
