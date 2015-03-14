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

#include "chunk.h"
#include "note.h"
#include "debugging.h"
#include <QDebug>

namespace Holdsworth {

    Chunk::Chunk()
	: notes_()
	, position_(NotDefined)
	, cost_(0)
    {
	/*
	 * Nothing
	 */
    }

    int Chunk::cost() const
    {
	return cost_;
    }

    unsigned int Chunk::length() const
    {
	return notes_.size();
    }

    void Chunk::addCost(int the_added_cost)
    {
	cost_ += the_added_cost;
    }

    void Chunk::addNote(const Note& the_note){
	notes_.push_back(the_note);
    }

    void Chunk::dbgDump() const
    {
#ifdef EXTRA_DEBUG

	qDebug("~~~~~~~~~~~~~~~~");
	qDebug("Dump of chunk @ posn %d:", position_);
	dbgDumpNoteList(notes_);
	qDebug("~~~~~~~~~~~~~~~~");
#endif
    }
    
    std::string Chunk::lilypondFretDiagram() const
    {
        QString s("\\fret-diagram #\"s:1;f:1;");
#ifdef EXTRA_DEBUG
        qDebug("lilypondFretDiagram");
#endif
        for (NoteList::const_iterator n = notes_.begin();
                n != notes_.end();
                ++n)
        {
            if ((*n).noteNum() == 0) {
                continue;
            }
            QString fingerdot;
            fingerdot.sprintf("%d-%d-%d;", 7 - (*n).stringNum(), (*n).fretNum(), (*n).fingerNum());
            if (!(*n).hasAnnotation(ANNO_QSHIFT)) {
                if (!s.contains(fingerdot)) {
                    s += fingerdot;
                }
            }
        }

        s += "\"";
#ifdef EXTRA_DEBUG
        qDebug() << s;
#endif
        return std::string(s.toLatin1().data());
    }

    void Chunk::makeFretDiag()
    {
        if (notes_.size() > 2) {
            notes_.front().setDiagram(lilypondFretDiagram());
        }
    }

    void Chunk::tagPositionShift()
    {
        if (!notes_.empty()) {
            if (notes_.front().hasAnnotation(HINT_SHIFT_UP)) {
		;
            }
            else if (notes_.front().hasAnnotation(HINT_SHIFT_DOWN)) {
		;
            }
            else {
                notes_.front().addAnnotation(ANNO_SHIFT);
            }
        }
    }
}
