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

#include "note.h"
#include <qstring.h>

#define STRING_HINT_RESTART	"="
#define STRING_HINT_SHIFT_UP	"+"
#define STRING_HINT_SHIFT_DOWN	"-"
#define STRING_HINT_BREAK	"*"
#define STRING_HINT_GLISS	"/"
#define STRING_ANNO_AUTOHINT	"c"
#define STRING_ANNO_STRETCH	"s"
#define STRING_ANNO_SHIFT	"*"
#define STRING_ANNO_LAYOVER	"L"
#define STRING_ANNO_BADCHANGE	"!"
#define STRING_ANNO_BADSTRETCH	"X"
#define STRING_ANNO_QSHIFT	"Q"
#define STRING_ANNO_TMOVE  	"T"
#define STRING_ANNO_OMOVE  	"O"
#define STRING_ANNO_AMOVE  	"A"

namespace Holdsworth {


    Note::Note()
	: note_num_(0)
	, duration_(240)
	, time_(NotDefined)
	, f_()
    {
    }

    Note::Note(NoteNum the_note_num)
	: note_num_(the_note_num)
	, duration_(240)
	, time_(NotDefined)
	, f_()
    {
    }

    Note::Note(NoteNum the_note_num, int the_duration, int the_time)
	: note_num_(the_note_num)
	, duration_(the_duration)
	, time_(the_time)
	, f_()
    {
    }

    void Note::purgeAutoHints()
    {
        if (f_.hasAnnotation(ANNO_AUTOHINT)) {
            f_.annotation = ANNO_NONE;
        }
    }

    bool Note::hasGlissHint() const
    {
        return (f_.hasAnnotation(HINT_GLISS));
    }

    bool Note::hasShiftHint() const
    {
        return (f_.hasAnnotation(HINT_SHIFT_UP)
                || f_.hasAnnotation(HINT_SHIFT_DOWN));
    }

    bool Note::hasRestartHint() const
    {
        return (f_.hasAnnotation(HINT_RESTART));
    }

    bool Note::hasBreakHint() const
    {
        return (f_.hasAnnotation(HINT_SHIFT_UP)
                || f_.hasAnnotation(HINT_SHIFT_DOWN)
                || f_.hasAnnotation(HINT_RESTART)
                || f_.hasAnnotation(HINT_BREAK)
                );
    }

    Annotation Note::annotationFromStr(const std::string& a)
    {
	if (a == STRING_HINT_RESTART)     return HINT_RESTART;
	if (a == STRING_HINT_SHIFT_UP)    return HINT_SHIFT_UP;
	if (a == STRING_HINT_SHIFT_DOWN)  return HINT_SHIFT_DOWN;
	if (a == STRING_HINT_GLISS)       return HINT_GLISS;
	if (a == STRING_ANNO_AUTOHINT)    return ANNO_AUTOHINT;
	if (a == STRING_ANNO_STRETCH)     return ANNO_STRETCH;
	if (a == STRING_ANNO_SHIFT)       return ANNO_SHIFT;
	if (a == STRING_ANNO_LAYOVER)     return ANNO_LAYOVER;
	if (a == STRING_ANNO_BADCHANGE)   return ANNO_BADCHANGE;
	if (a == STRING_ANNO_BADSTRETCH)  return ANNO_BADSTRETCH;
	if (a == STRING_ANNO_QSHIFT)      return ANNO_QSHIFT;
	if (a == STRING_ANNO_TMOVE)       return ANNO_TMOVE;
	if (a == STRING_ANNO_OMOVE)       return ANNO_OMOVE;
	if (a == STRING_ANNO_AMOVE)       return ANNO_AMOVE;
	if (a == STRING_HINT_BREAK)       return HINT_BREAK;

	return ANNO_NONE;
    }

    std::string Note::annotationAsStr() const
    {
	QString s;


	if (f_.hasAnnotation(HINT_RESTART	)) s += STRING_HINT_RESTART;
	if (f_.hasAnnotation(HINT_SHIFT_UP	)) s += STRING_HINT_SHIFT_UP;
	if (f_.hasAnnotation(HINT_SHIFT_DOWN	)) s += STRING_HINT_SHIFT_DOWN;
	if (f_.hasAnnotation(HINT_GLISS		)) s += STRING_HINT_GLISS;
	if (f_.hasAnnotation(HINT_BREAK		)) s += STRING_HINT_BREAK;
	if (f_.hasAnnotation(ANNO_AUTOHINT	)) s += STRING_ANNO_AUTOHINT;
	if (f_.hasAnnotation(ANNO_STRETCH	)) s += STRING_ANNO_STRETCH;
	if (f_.hasAnnotation(ANNO_SHIFT		)) s += STRING_ANNO_SHIFT;
	if (f_.hasAnnotation(ANNO_LAYOVER	)) s += STRING_ANNO_LAYOVER;
	if (f_.hasAnnotation(ANNO_BADCHANGE	)) s += STRING_ANNO_BADCHANGE;
	if (f_.hasAnnotation(ANNO_BADSTRETCH	)) s += STRING_ANNO_BADSTRETCH;
	if (f_.hasAnnotation(ANNO_QSHIFT	)) s += STRING_ANNO_QSHIFT;
	if (f_.hasAnnotation(ANNO_TMOVE  	)) s += STRING_ANNO_TMOVE;
	if (f_.hasAnnotation(ANNO_OMOVE  	)) s += STRING_ANNO_OMOVE;
	if (f_.hasAnnotation(ANNO_AMOVE  	)) s += STRING_ANNO_AMOVE;

	if (s.isEmpty()) {
	    return std::string();
	}
	else {
	    return std::string(s.toLatin1().data());
	}
    }

    std::string Note::dbgDump() const
    {
	QString d;

	d.sprintf("notenum: %d duration: %d time: %d string: %d fret: %d finger: %d ", 
		note_num_,
		duration_,
		time_,
		f_.strg,
		f_.fret,
		f_.finger);

        d += QString(annotationAsStr().c_str());

	return std::string(d.toLatin1().data());
    }
}
