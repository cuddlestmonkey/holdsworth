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

#include <qglobal.h>
#include "handmodelx.h"

namespace Holdsworth {

    HandModelX::HandModelX()
        : HandModel()
    {
        /* Nothing */
    }
    
    FingerPositionList HandModelX::candidates(const FretPos& x)
    {
#ifdef EXTRA_DEBUG
        qDebug("FingerPositionList HandModelX::candidates for fretpos= s: %d f: %d", x.strg, x.fret);
#endif
        FingerPositionList fpl;

        int fret = x.fret + 1;

        while ((fret > 0) && (fpl.size() < 5)) {
#ifdef EXTRA_DEBUG
            qDebug("Adding %d as a possible position", fret);
#endif
            fpl.push_back(fret);
            --fret;
        }
        return fpl;
    }
    
    FingerNum HandModelX::getFinger(FretNum f, FingerPosition start_p)
    {
        FingerNum finger = NoFingerDefined;
        switch (f - start_p) {
            case -1:
            case 0:
                finger = FirstFinger;
                break;

            case 1:
                finger = MiddleFinger;
                break;

            case 2:
                finger = RingFinger;
                break;

            case 3:
                finger = LittleFinger;
                break;

            default:
                qDebug("POSITION breakdown: diff = %d", f - start_p);
        }

        return finger;
    }

    bool HandModelX::isStretch(FretNum f, FingerPosition start_p)
    {
        switch (f - start_p) {
            case -1:
                return true;
                break;

            case 0:
            case 1:
            case 2:
            case 3:
                return false;
                break;

            default:
                return true;
        }
    }
};
