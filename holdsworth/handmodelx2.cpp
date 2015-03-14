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
#include "handmodelx2.h"

namespace {
    /*! \brief The penalty assigned to taking a note on the little finger.
     *
     * Other things being equal, it is nice to avoid notes on the little finger
     * as some people find it awkward. So we assign a small penalty to pinky
     * notes. It is only small though, to act as a tiebreaker.  Taking a note
     * on the pinky is always better than some other nasty position shift.
     */
    const int vn_pinky_penalty = 1;

    /*! \brief The penalty assigned to using a first finger extension.
     */
    const int vn_index_stretch_penalty = 1;
    
    /*! \brief The penalty assigned to using a little finger extension. This is in
     * addition to the usual penalty.
     */
    const int vn_pinky_stretch_penalty = 1;
}

namespace Holdsworth {

    HandModelX2::HandModelX2()
        : HandModel()
    {
        /* Nothing */
    }
    
    FingerPositionList HandModelX2::candidates(const FretPos& x)
    {
#ifdef EXTRA_DEBUG
        qDebug("FingerPositionList HandModelX2::candidates for fretpos= s: %d f: %d", x.strg, x.fret);
#endif
        FingerPositionList fpl;

        int fret = x.fret + 1;

        while ((fret > 0) && (fpl.size() < 6)) {
#ifdef EXTRA_DEBUG
            qDebug("Adding %d as a possible position", fret);
#endif
            fpl.push_back(fret);
            --fret;
        }
        return fpl;
    }
    
    FingerNum HandModelX2::getFinger(FretNum f, FingerPosition start_p)
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
	    case 4:
                finger = LittleFinger;
                break;

            default:
                qDebug("POSITION breakdown: diff = %d", f - start_p);
        }

        return finger;
    }

    bool HandModelX2::isStretch(FretNum f, FingerPosition start_p)
    {
        switch (f - start_p) {
            case -1:
	    case 4:
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

    int HandModelX2::cost(const Fingering& f, FingerPosition p)
    {
        int c = 0;

        if (f.finger == LittleFinger) {
            c += vn_pinky_penalty;
            if (isStretch(f.fret, p)) {
                c += vn_pinky_stretch_penalty;
            }
        }
        else if (isStretch(f.fret, p)) {
            c += vn_index_stretch_penalty;
        }

        return c;
    }
};
