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

#ifndef HOLDSWORTH_CONSTRAINTS_H
#define HOLDSWORTH_CONSTRAINTS_H


namespace Holdsworth {


/*!
 * \brief Holder class for user-selectable constraints.
 *
 * This class holds constraints that are beyond those dictated by the instrument
 * or the basic algorithm. Examples:
 *
 * 1) Don't put notes in the first N frets (favour high neck fingerings)
 * 2) Don't use layovers
 * 3) Don't do extensions on the bottom E string
 *
 * The default constructor for the class sets it up so as to impose
 * NO constraints. So all members of the nature "is X allowed" are initialised
 * to true.
 */
class Constraints
{
public:
    Constraints()
        : btb_gliss_allowed_(true)
        {/*empty*/}

    virtual ~Constraints() {/*empty*/}


    void setBTBGliss(bool is_allowed) {btb_gliss_allowed_ = is_allowed;}
    bool getBTBGliss() {return btb_gliss_allowed_;}

private:
    bool btb_gliss_allowed_;
};

}
#endif /* HOLDSWORTH_CONSTRAINTS_H */
