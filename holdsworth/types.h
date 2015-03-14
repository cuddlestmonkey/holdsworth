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

#ifndef HOLDSWORTH_TYPES_H
#define HOLDSWORTH_TYPES_H

#include <vector>

/*!
 * \brief All the library routines and classes are in this namespace, to ease use with
 * other musical software.
 */
namespace Holdsworth {

    /*!
     * \brief Generic value for "not yet fixed"/"doesn't matter"
     */
    const int NotDefined = -1;

    /*! \brief MIDI note number
     *
     */
    typedef int NoteNum;
    /*! \brief Fret number (0 = open string) */
    typedef int FretNum;
    /*! \brief String number (1 = lowest.) */
    typedef int StringNum;

    /*! \brief A left-hand position (right if you're left-handed)
     *
     * Traditionally this represents the fret of the first finger. In some approaches
     * though the first finger "roams" over a two-fret span, so then the "position" is
     * a little more subtly defined as the fret before the fret of the second finger.
     */
    typedef int FingerPosition;

    /*! \brief Enum of left hand fingers
     */
     enum FingerNum {
        NoFingerDefined = -1,
        OpenString = 0,         /*!< Note on an open string */
        FirstFinger = 1,        /*!< First finger (Index) */
        MiddleFinger = 2,       /*!< Second finger (Middle) */
        SecondFinger = 2,       /*!< Second finger (Middle) */
        RingFinger = 3,         /*!< Third finger (Ring) */
        ThirdFinger = 3,         /*!< Third finger (Ring) */
        LittleFinger = 4,        /*!< Fourth finger (Little, Pinky) */
        FourthFinger = 4        /*!< Fourth finger (Little, Pinky) */
    };

    /*! \brief A position on the fretboard.
     */
    struct FretPos {
        StringNum       strg;   /*!< Number of the string */
        FretNum         fret;   /*!< Fret number (0= open string) */

        /*! Convenience constructor
         */
        FretPos(StringNum sn, FretNum fn)
            : strg(sn)
            , fret(fn)
            {}

        /*! Default Constructor
         */
        FretPos()
            : strg(NotDefined)
            , fret(NotDefined)
            {}
    };


    enum Annotation {
	HINT_SHIFT_UP   = 0x0001, //"+";
	HINT_SHIFT_DOWN = 0x0002, //"-";
	HINT_RESTART    = 0x0004, //"=";
	HINT_GLISS      = 0x0008, //"/";
	ANNO_SHIFT      = 0x0010, //"*";
	ANNO_STRETCH	= 0x0020, //"s";
	ANNO_LAYOVER	= 0x0040, //"L";
	ANNO_BADCHANGE	= 0x0080, //"!";
	ANNO_BADSTRETCH	= 0x0100, //"X";
	ANNO_QSHIFT	= 0x0200, //"Q";
	ANNO_TMOVE  	= 0x0400, //"T";
	ANNO_OMOVE  	= 0x0800, //"O";
	ANNO_AMOVE  	= 0x1000, //"A";

	ANNO_AUTOHINT	= 0x2000, //"c";
        HINT_BREAK      = 0x4000, //"*";

	ANNO_NONE	= 0x0000
    };

    /*! \brief A fingering detail, extending a fretboard position with
     * additional information (such as the finger used)
     */
    struct Fingering : public FretPos {
        FingerNum   finger;     /*!< Finger used */
        Annotation  annotation;	/*!< Annotation(s) */

        Fingering()
            : FretPos()
            , finger(NoFingerDefined)
            , annotation(ANNO_NONE)
            {}

	void addAnnotation(const Annotation x) {annotation = (Annotation) (annotation | x);}
	bool hasAnnotation(const Annotation x) const {return ((annotation & x) != 0);}
    };


    /*! \brief An unordered list of FingerPositions. */
    typedef std::vector<FingerPosition> FingerPositionList;

    /*! \brief An unordered list of fretboard positions.
     *
     * Normally this is used in the context of returning a list of
     * fretboard positions that produce the same note.
     */
    typedef std::vector<FretPos> FretPosList;


}
#endif /* HOLDSWORTH_TYPES_H */
