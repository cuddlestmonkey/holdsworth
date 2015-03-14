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
#include "algorithm.h"
#include "instrumentdefn.h"
#include "handmodel.h"

namespace Holdsworth {

    Algorithm::Algorithm()
	: instrument_(0)
	, constraints_(0)
	, handmodel_(0)
    {
    }

    void Algorithm::setInstrument(InstrumentDefn *the_instrument)
    {
	Q_ASSERT(the_instrument != 0);
	instrument_ = the_instrument;
    }

    void Algorithm::setConstraints(Constraints *the_constraints)
    {
	Q_ASSERT(the_constraints != 0);
	constraints_ = the_constraints;
    }

    void Algorithm::setHandModel(HandModel *the_model)
    {
	Q_ASSERT(the_model != 0);
	handmodel_ = the_model;
    }

    FingerPositionList Algorithm::candidates(const FretPos& x)
    {
#ifdef EXTRA_DEBUG
	qDebug("FingerPositionList Algorithm::candidates for fretpos= s: %d f: %d", x.strg, x.fret);
#endif
	return handmodel_->candidates(x);
    }

    FretPosList Algorithm::candidates(const Note& y) 
    {
	return instrument_->candidates(y);
    }
}
