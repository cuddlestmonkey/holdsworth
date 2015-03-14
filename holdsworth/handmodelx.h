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

#ifndef HOLDSWORTH_HANDMODELX_H
#define HOLDSWORTH_HANDMODELX_H

#include <holdsworth/types.h>
#include <holdsworth/note.h>
#include <holdsworth/handmodel.h>

namespace Holdsworth {

/*!
 * \brief Model of the capabilities of the left hand (note span etc.) that
 * allows first-finger extensions.
 *
 */
class HandModelX : public HandModel
{
public:
    HandModelX();
    virtual ~HandModelX() {/*empty*/}


    /*! \brief Return candidate LH positions for a given position on the
     * fingerboard.
     *
     * A candidate position is one in which the given fret/string falls under the reach of the LH.
     */
    virtual FingerPositionList candidates(const FretPos&);

    /*! \brief Return the finger that is used for a given fret in a given
     * position.
     *
     * \todo Strictly, this should return a candidate list. At the moment
     * we are assuming a strict "each fret is only covered by one finger" model.
     */
    virtual FingerNum getFinger(FretNum, FingerPosition);

    /*! \brief Indicate if reaching the given fret from the given position
     * involves a stretch.
     *
     * \todo There should be a variant of this which passes in the finger. At the moment
     * we are assuming a strict "each fret is only covered by one finger" model.
     */
    virtual bool isStretch(FretNum, FingerPosition);

};

}
#endif /* HOLDSWORTH_HANDMODELX_H */
