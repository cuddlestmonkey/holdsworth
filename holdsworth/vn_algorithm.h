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

#ifndef HOLDSWORTH_VN_ALGORITHM_H
#define HOLDSWORTH_VN_ALGORITHM_H


#include <holdsworth/algorithm.h>

namespace Holdsworth {

/*!
 * \brief Implementation of VN's algorithm that plots out feasible fingerings.
 *
 */
class VNAlgorithm : public Algorithm
{
public:
    VNAlgorithm()
	: Algorithm()
	{}

    /*!
     * \brief Build a block of fingering.
     *
     * The algorithm is expected to return the largest feasible chunk
     * of fingerings possible without abandoning position.
     */
    virtual Chunk generateChunk(ConstNoteIterator, const FingerPosition&, const FretPos&, const Fingering& force_first, const Note *lead_in);


private:

};

}
#endif /* HOLDSWORTH_VN_ALGORITHM_H */
