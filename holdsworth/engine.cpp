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

#include "engine.h"
#include "debugging.h"
#include <iostream>
#include <QDebug>
extern bool quiet;
/*! \todo this should go into Constraints */

#define OUTPUT  if (!quiet) std::cout 


namespace Holdsworth {

    /*! \brief Penalty for re-using the same position when we've been told to
     * find a new one.
     *
     * If a position break hint has been inserted, it means that the current
     * position is not satisfactory. However, left to its own devices, the
     * algorithm may recommend staying in the same position. Since a break hint
     * means we think we know better, we give suggestions from the algorithm
     * that don't change position a penalty. A penalty is used, rather than a
     * complete ban, because it is possible that staying in the same position
     * is the least worst option.
     */
    const int engine_position_repeat_deter = 0;

    /*! \brief Maximum size of a position shift (in frets) that we will accept
     * before invoking the auto-hinter.
     *
     * All algorithms are expected to be "maximal munch"; given a starting
     * position, they should return as many notes as they can sensibly render
     * in that position.  This means that when a position shift is finally
     * inevitable, the jump can be quite large.  If a jump is too large, then
     * the engine will look back along the rendered chunk and locate a sensible
     * place to put a position break hint.
     */
    const int dflt_engine_max_lh_shift = 3;

    /*! \brief The size of position shift considered "ideal"
     *
     * i.e. Not too large to be comfortable, but not so small as to be
     * pointless.
     */
    const int dflt_engine_nice_lh_shift = dflt_engine_max_lh_shift / 2;

    /*! \brief The smallest size of chunk in which autohints can be inserted.
     *
     * If there is a genuinely big position shift inherent in the input line,
     * then no amount of break hinting can avoid it. This threshold stops the
     * engine persisting in a futile attempt to square the circle.
     *
     * This is a "seems to work" value and some experiment is needed to find the
     * best one. However, once discovered it is likely this value will hold for
     * most algorithms.
     */
    const uint engine_auto_hint_giveup_size = 12;

    static inline int absolute_diff(FingerPosition a, FingerPosition b)
    {
        return ((a > b) ? (a - b) : (b - a));
    }

    Engine::Engine()
	: instrument_(0)
	, constraints_(0)
	, algorithm_(0)
        , nlist_()
        , source_note_list_()
        , hint_location_()
        , hint_type_()
        , max_lh_shift(dflt_engine_max_lh_shift)
	{/*empty*/}
    
    bool Engine::compute(const NoteList& source_notelist, int max_pass)
    {
#ifdef EXTRA_DEBUG
        qDebug("Engine::compute");
#endif
        int pass_num = 0;
        /*
         * Take a copy of the input list, for autohint insertion.
         * 
         * QValueList is implicitly shared, so this is ok
         */
        source_note_list_ = source_notelist;
        
#ifdef EXTRA_DEBUG
        dbgDumpNoteList(source_notelist);
#endif
        /*
         * Check that we have the necessary information
         */
        if (instrument_ == 0) {
            qDebug("instrument_ not set!");
            return false;
        }
        if (algorithm_ == 0) {
            qDebug("algorithm_ not set!");
            return false;
        }
        if (constraints_ == 0) {
            qDebug("constraints_ not set!");
            return false;
        }

        do {
            OUTPUT << "Pass: " << ++pass_num << ": ";
            /*
             * Clear out any old notelist
             */
            nlist_.clear();

            /*
             * Start from the first note in the list
             */
            ConstNoteIterator cni = source_note_list_.begin();
            while (cni != source_note_list_.end() && ((*cni).noteNum() != NotDefined) && ((*cni).noteNum() == 0)) {
                /* Pass through initial rests in pick-up bar */
                Note n;
                n.setDuration((*cni).duration());

                nlist_.push_back(n);

                /*
                 * ...and onto the next note
                 */
                ++cni;
            }

            ConstNoteIterator start_of_last_chunk = cni;

            FingerPosition  last_fp = NotDefined;

            hint_type_ = ANNO_NONE;

            while (cni != source_note_list_.end() && ((*cni).noteNum() != NotDefined)) {
                int last_cost = 1000000L;
                Chunk bestchunk;

                /*
                 * Locate possible positions for this note on the fretboard
                 */
                FretPosList fpcandidates = instrument_->candidates(*cni);

                if (fpcandidates.empty()) {
                    qDebug("No possible starting position!");
                    return false;
                }

                Fingering cf = (*cni).fingering();
                const Note *lead_in_note = 0;

                if (cni != source_note_list_.begin()) {
                    lead_in_note = &nlist_.back();
                    if (cf.hasAnnotation(HINT_GLISS)) {
                        cf.finger = lead_in_note->fingerNum();
                        cf.strg = lead_in_note->stringNum();
                    }
                }
                
                /*
                 * The restart ("=") hint means we should ignore the lead-in
                 * note - imagine that we are starting afresh.
                 */
                if ((*cni).hasRestartHint()) { lead_in_note = 0; }
                
                /*
                 * For each possible starting position...
                 */
                for (FretPosList::const_iterator fp = fpcandidates.begin();
                        fp != fpcandidates.end();
                        ++fp)
                {
                    /*
                     * Has the string been mandated?
                     */
                    if ((cf.strg != NotDefined) && (cf.strg != (*fp).strg)) {
#ifdef EXTRA_DEBUG
                        qDebug("Auto-string for chunk start overridden by input hint");
#endif
                        continue;
                    }
                    /*
                     * Locate the possible corresponding starting LH position
                     */
                    FingerPositionList pcandidates = algorithm_->candidates(*fp);
                    
                    /*
                     * For each of these possible LH positions....
                     */
                    for (FingerPositionList::const_iterator p = pcandidates.begin();
                            p != pcandidates.end();
                            ++p)
                    {
                        /*
                         * Generate a fingering chunk
                         */
                        Chunk c = algorithm_->generateChunk(ConstNoteIterator(cni), *p, *fp, cf, lead_in_note);
#ifdef SOME_DEBUG
                        qDebug("Chunk @%d cost = %d", c.getPosition(), c.cost());
#endif

                        if ((last_fp != NotDefined) && (lead_in_note != 0)) {
                            if ((*cni).hasAnnotation(HINT_SHIFT_UP)) {
                                c.addCost(hintUpCost(last_fp, *p));
                            }
                            else if ((*cni).hasAnnotation(HINT_SHIFT_DOWN)) {
                                c.addCost(hintDownCost(last_fp, *p));
                            }
                            else {
                                /*
                                 * Add a penalty for large position skips.
                                 * In other words, if two starting positions yield similar
                                 * results, then the one that involves the smaller movement
                                 * from the last position is better.
                                 */
                                c.addCost(absolute_diff(*p, last_fp));
                            }
                        }
                        else {
                            if ((*cni).hasAnnotation(HINT_SHIFT_UP)) {
                                c.addCost(hintUpCost(*p));
                            }
                            else if ((*cni).hasAnnotation(HINT_SHIFT_DOWN)) {
                                c.addCost(hintDownCost(*p));
                            }
                            else {
                                /*
                                 * Prefer a lower position, but don't be silly about it.
                                 */
                                if (*p > 7) {
                                    c.addCost(*p - 7);
                                }
                                if (*p < 5) {
                                    c.addCost(5 - *p);
                                }
                            }

                            if (c.length() == 1) {
                                /*
                                 * Don't allow the bonuses for low positions to fool us
                                 * into accepting a rubbishy one-note segment.
                                 */
                                c.addCost(5); //TODO
                            }

                        }
#ifdef SOME_DEBUG
                        qDebug("Added position cost, final = %d", c.cost());
#endif
                        if ((c.cost() < last_cost) 
                            || ((c.cost() == last_cost) && (c.length() > bestchunk.length()))) {
                            last_cost = c.cost();
#ifdef SOME_DEBUG
                            qDebug("We have a new best chunk with cost %d!!!", last_cost);
#endif
                            bestchunk = c;
                        }
#ifdef EXTRA_DEBUG
                        qDebug("===================\n\n");
#endif
                    }
                }

                /*
                 * Append the best chunk
                 */
#ifdef EXTRA_DEBUG
                qDebug("Adding %d notes from best chunk with score %d", bestchunk.length(), bestchunk.cost());
                bestchunk.dbgDump();
#endif
                bestchunk.makeFretDiag();
                
                if ((*cni).hasRestartHint()) {
                    OUTPUT << " ";
                }
                FingerPosition bp = bestchunk.getPosition();
                OUTPUT << "[" << bp << "]";
                /*
                 * It is possible that the best chunk represents a big position skip
                 * from the previous chunk. This can indicate a "maximum munch" problem
                 * wherein the previous chunk over-milked a position.
                 */
                if ((last_fp != NotDefined) && (last_fp != bp) && (hint_type_ == ANNO_NONE)) {
                    int pdiff = absolute_diff(last_fp, bp);

                    if (
                        ((pdiff > max_lh_shift) && (!(*cni).hasRestartHint()))
                        ||
                        (!constraints_->getBTBGliss() && (bestchunk.length() == 1) && (!(*cni).hasGlissHint()))
                    ) {
                        OUTPUT << "<" << pdiff << ">";
#ifdef EXTRA_DEBUG
                        qDebug("***Excessive LH shift - try finding shift hint locations ***");
#endif
                        if (last_fp > bp) {
                            hint_type_ = HINT_SHIFT_DOWN;
                        }
                        else {
                            hint_type_ = HINT_SHIFT_UP;
                        }
                            
                        bool lever_chunk_start = false;

                        if (start_of_last_chunk == source_note_list_.begin()) {
                            if (!(*start_of_last_chunk).hasShiftHint()) {
                                OUTPUT << "<liststart>";
                                hint_location_ = start_of_last_chunk;
                                lever_chunk_start = true;
                            }
                        }
                        else if ((*start_of_last_chunk).hasRestartHint()) {
                            if (!(*start_of_last_chunk).hasShiftHint()) {
                                OUTPUT << "<chunkstart>";
                                hint_location_ = start_of_last_chunk;
                                lever_chunk_start = true;
                            }
                        }
                        
                        if (!lever_chunk_start) {
                            /*
                             * If there is a small jump, don't use shift hints, they do more harm than good.
                             */
                            if (pdiff < 5) {
                                hint_type_ = HINT_BREAK;
                            }
                            /*
                             * Pass 1 - determine note range of previous chunk
                             */
                            NoteNum lowest_note = 10000;
                            NoteNum highest_note = 0;
                            uint note_count = 0;
                            //QT4: we can do ni = cni - 1
                            ConstNoteIterator ni = cni;
                            --ni;
                                
                            for (; ni != start_of_last_chunk; --ni) {
                                if ((*ni).noteNum() < lowest_note) {
                                    lowest_note = (*ni).noteNum();
                                }
                                if ((*ni).noteNum() > highest_note) {
                                    highest_note = (*ni).noteNum();
                                }
                                ++note_count;
                            }

                            NoteNum threshold_note = (lowest_note + highest_note) / 2;
                            if (pdiff == 5) {
                                //OUTPUT << "{" << note_count << "/" << highest_note - lowest_note << "}";  
                                if ((highest_note > lowest_note)) {
                                    if ((note_count / (highest_note - lowest_note)) < 2) {
                                        hint_type_ = HINT_BREAK;
                                    }
                                }
                            }

                            if (hint_type_ == HINT_SHIFT_UP) {
                                OUTPUT << "+";
                            }
                            else if (hint_type_ == HINT_SHIFT_DOWN) {
                                OUTPUT << "-";
                            }
                            else if (hint_type_ == HINT_BREAK) {
                                OUTPUT << "*";
                            }

                            if (note_count < engine_auto_hint_giveup_size) {
                                OUTPUT << "!";
                                hint_type_ = ANNO_NONE;
#ifdef EXTRA_DEBUG
                                qDebug("Tough, this looks like a genuine case of nasty shifts");
#endif
                            }
                            else {
#ifdef EXTRA_DEBUG
                                qDebug("Note range is %d to %d", lowest_note, highest_note);
#endif

                                /*
                                 * Pass 2 - determine location of hint
                                 */
                                QList<ConstNoteIterator> crossing_points;
                                ConstNoteIterator ni = cni;
                                --ni;
                                int last_note = (*ni).noteNum();
                                while (ni != start_of_last_chunk) {
                                    if (((*ni).noteNum() <= threshold_note) && (last_note > threshold_note)) {
                                        if (hint_type_ != HINT_SHIFT_DOWN) {
                                            crossing_points.append(ni);
                                        }
                                    }
                                    else if (((*ni).noteNum() > threshold_note) && (last_note <= threshold_note)) {
                                        if (hint_type_ != HINT_SHIFT_UP) {
                                            crossing_points.append(ni);
                                        }
                                    }

                                    last_note = (*ni).noteNum();
                                    --ni;
                                }
                                OUTPUT << "(" << crossing_points.count() << " crossings)";

                                if (crossing_points.count() == 0) {
                                    hint_type_ = ANNO_NONE;
                                }
                                else if (note_count / crossing_points.count() < 4) {
                                    hint_type_ = ANNO_NONE;
                                }
                                else {
                                    ni = crossing_points[crossing_points.count() / 2];
                                }

                                hint_location_ = ni;
                                ++hint_location_;
                                if ((*hint_location_).isRest()) {
                                    --hint_location_;
                                }
#ifdef EXTRA_DEBUG
                                qDebug("Suggest Split at the note:-");
                                qDebug((*hint_location_).dbgDump().c_str());
#endif
                            }
                        }
                    }
                }
                
                /*
                 * Indicate position change
                 */
                if (last_fp == NotDefined) {
                    last_fp = bp;
                }
                else if (last_fp != bp) {
                    last_fp = bp;
                    bestchunk.tagPositionShift();
                }

		// std:list doesn't have the semantic sugar of += on lists. Hmph.
		// The following is nlist_ += bestchunk.noteList().
		
		for (ConstNoteIterator foobar = bestchunk.noteList().begin();
		      foobar != bestchunk.noteList().end();
		      ++foobar
		) {
		    nlist_.push_back(*foobar);
		}

                if ((*cni).hasBreakHint()) {
                    start_of_last_chunk = cni;
                }          
                else if (bestchunk.length() > engine_auto_hint_giveup_size) {
                    start_of_last_chunk = cni;
                }

                /*
                 * Skip past the chunked notes, making sure we don't go off the end of the list.
                 * Remember where this chunk started.
                 */
                for (unsigned int i = 0; (i < bestchunk.length()) && (cni != source_note_list_.end()); ++i) {
                    ++cni;
                }
#ifdef EXTRA_DEBUG
                qDebug("-------------------------------\n\n");
#endif

                /*
                 * Don't bother going further if we have hit a restart
                 * point and the notelist up to now already has a hint
                 * inserted.
                 */
                if ((*cni).hasRestartHint() && (hint_type_ != ANNO_NONE)) {
                    break;
                }
            }

            if (hint_type_ != ANNO_NONE) {
                bool purge = false;
                for (NoteList::iterator ni = source_note_list_.begin(); ni != source_note_list_.end(); ++ni) {
                    if (ConstNoteIterator(ni) == hint_location_) {
                        purge = true;
                        (*ni).addAnnotation(hint_type_);
                        (*ni).addAnnotation(ANNO_AUTOHINT);
                    }
                    else if (purge) {
                        (*ni).purgeAutoHints();
                        if ((*ni).hasRestartHint()) {
                            purge = false;
                        }

                    }
                }
            }
            OUTPUT << " Done." << std::endl;

        } while ((hint_type_ != ANNO_NONE) && (pass_num < max_pass));
        
        return true;
    }

    int Engine::hintUpCost(FingerPosition last_fp, FingerPosition new_fp)
    {
        int c = 0;
        /*
         * This is a hinted position jump.
         * Therefore we deter selection of the same position,
         */
        if (new_fp == last_fp) {
            c += engine_position_repeat_deter;
        }
        /*
         * Prefer a higher position
         */
        FingerPosition target_fp = last_fp + dflt_engine_nice_lh_shift;
        c += absolute_diff(new_fp, target_fp);
        return c;
    }

    int Engine::hintUpCost(FingerPosition new_fp)
    {
        int c = 0;
        /*
         * Prefer a higher position
         */
        c += (24 - new_fp);
        c += (24 - new_fp);
        c += absolute_diff(new_fp, 7);
        return c;
    }

    int Engine::hintDownCost(FingerPosition last_fp, FingerPosition new_fp)
    {
        int c = 0;
        /*
         * This is a hinted position jump.
         * Therefore we deter selection of the same position,
         */
        if (new_fp == last_fp) {
            c += engine_position_repeat_deter;
        }
        /*
         * Prefer a lower position
         */
        FingerPosition target_fp = last_fp - dflt_engine_nice_lh_shift;
        c += absolute_diff(new_fp, target_fp);
        return c;
    }

    int Engine::hintDownCost(FingerPosition new_fp)
    {
        int c = 0;
        /*
         * Prefer a lower position
         */
        c += (new_fp);
#if 0
        c += (new_fp);
        c += absolute_diff(new_fp, 7);
#endif
        return c;
    }

    void Engine::setInstrument(InstrumentDefn *the_instrument)
    {
        Q_ASSERT(the_instrument != 0);
        instrument_ = the_instrument;

        if (algorithm_ != 0) {
            algorithm_->setInstrument(instrument_);
        }
    }

    void Engine::setConstraints(Constraints *the_constraints)
    {
        Q_ASSERT(the_constraints != 0);
        constraints_ = the_constraints;

        if (algorithm_ != 0) {
            algorithm_->setConstraints(constraints_);
        }
    }
    
    void Engine::setAlgorithm(Algorithm *the_algorithm)
    {
        Q_ASSERT(the_algorithm != 0);
        algorithm_ = the_algorithm;

        if (instrument_ != 0) {
            algorithm_->setInstrument(instrument_);
        }
        if (constraints_ != 0) {
            algorithm_->setConstraints(constraints_);
        }
    }

    void Engine::dumpLilyPond(QTextStream &os, bool use_flats, bool show_annotations)
    {
        dbgLilypondDumpNoteList(nlist_, os, true, use_flats, show_annotations);
    }

    void Engine::dumpLilyPondTab(QTextStream &os)
    {
        dbgLilypondDumpNoteList(nlist_, os, false, false, false);
    }

}


