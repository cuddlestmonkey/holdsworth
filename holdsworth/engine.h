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

#ifndef HOLDSWORTH_ENGINE_H
#define HOLDSWORTH_ENGINE_H

#ifndef PURE_STL_INTERFACE
    #include <qtextstream.h>
#endif
#include <holdsworth/note.h>
#include <holdsworth/instrumentdefn.h>
#include <holdsworth/constraints.h>
#include <holdsworth/algorithm.h>

namespace Holdsworth {


/*!
 *  \brief Fingering generation engine. Main access point for the consumer of the library.
 *
 * The consumer of the library will typically:
 *
 * 1) Create an instance of Holdsworth::Engine
 *
 * 2) Set the appropriate instrument model & algorithm
 *
 * 3) Set any constraints (if desired)
 *
 * 4) Build a Holdsworth::NoteList from his internal MIDI note data
 *
 * 5) Call Engine::compute()
 *
 * 6) Retrieve the resulting chunk list
 *
 * 7) Extract the fingering data and use it as desired.
 */
class Engine
{
public:
    Engine();
    virtual ~Engine() {/*empty*/}

    /*! \brief Compute an optimal fingering for a set of note data.
     *
     * \param source_note_list_ The input list of notes
     * \param max_pass		Maximum number of passes to make. The engine may make less passes
     * than the maximum.
     *
     * Of course, the definition of "optimal" is what the algorithm considers
     * optimal.
     */
    bool compute(const NoteList& source_note_list_, int max_pass);

    /*! \brief Accessor function for output data.
     *
     * This only returns anything meaningful after a successful call to compute(). The
     * notelist returned has the same notes (in the same order) as the incoming notelist,
     * but with additional fingering and annotation information.
     */
    const NoteList& output() const {return nlist_;}

    void setInstrument(InstrumentDefn *);   /*!< Settor function for associated InstrumentDefn */
    void setConstraints(Constraints *);	    /*!< Settor function for associated Constraints */
    void setAlgorithm(Algorithm *);    	    /*!< Settor function for associated Algorithm */

    /* \brief Change the maximum allowed LH Shift.
     *
     * The default is dflt_engine_max_lh_shift.
     */
    void setMaxLHShift(int x) {max_lh_shift = x;}

#ifndef PURE_STL_INTERFACE
    /*! \brief Dump a Lilypond format stream of the rendered notes.
     *
     * \param use_flats Express accidental notes as flats rather than sharps
     *
     * This function dumps the notes in a format suitable for the regular Staff.
     */
    void dumpLilyPond(QTextStream &, bool use_flats, bool show_annotations);

    /*! \brief Dump a Lilypond format stream of the rendered notes, in tab
     *
     * This function dumps the notes in a format suitable for TabStaff.
     */
    void dumpLilyPondTab(QTextStream &);
#endif

protected:
    /*! \brief Defines the weighting given to various position shifts when 
     * hinting up.
     *
     * Override this function to change the auto-hint behaviour.
     */ 
    virtual int hintUpCost(FingerPosition last_fp, FingerPosition new_fp);
    
    /*! \brief Defines the weighting given to various position shifts when 
     * hinting down.
     *
     * Override this function to change the auto-hint behaviour.
     */ 
    virtual int hintDownCost(FingerPosition last_fp, FingerPosition new_fp);

    /*! \brief Defines the weighting given to various initial positions when 
     * hinting up.
     *
     * Override this function to change the auto-hint behaviour.
     */ 
    virtual int hintUpCost(FingerPosition new_fp);
    
    /*! \brief Defines the weighting given to various initial positions when 
     * hinting down.
     *
     * Override this function to change the auto-hint behaviour.
     */ 
    virtual int hintDownCost(FingerPosition new_fp);
    
private:
    InstrumentDefn      *instrument_;
    Constraints	        *constraints_;
    Algorithm	        *algorithm_;

    NoteList            nlist_;
    NoteList            source_note_list_;
    ConstNoteIterator   hint_location_;
    Annotation		hint_type_;

    /*! \brief Maximum size of a position shift (in frets) that we will accept
     * before invoking the auto-hinter. see dflt_engine_max_lh_shift.
     */
    int                 max_lh_shift;
};

}
#endif /* HOLDSWORTH_ENGINE_H */
