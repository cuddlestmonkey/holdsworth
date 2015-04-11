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

#include "vn_algorithm.h"
#include "instrumentdefn.h"
#include "handmodel.h"
#include "debugging.h"

namespace Holdsworth {

    /*! \brief The bonus assigned to adding a new note to a chunk.
     * 
     * This must always outweigh the "typical" cost of a note, or you will get
     * silly results.
     *
     * The bigger the bonus (the more negative the value), the more Holdsworth
     * will favour awkward finger moves that allow you stay in one position for
     * longer.
     */
    const int vn_note_bonus = -5;

    /*! \brief The maximum cost that can borne on a single note before it becomes worth
     * trying to break position.
     *
     * If the cost passes this level, we try position shifts instead.
     * where we currently are. It may turn out that staying in the same
     * position is still best.
     */
    const int vn_position_break_threshold = 15;

    /*! \brief The maximum cost that can be accumulated before it becomes worth
     * trying to break position.
     *
     * If the cost passes this level, we look afresh for a best position from
     * where we currently are. It may turn out that staying in the same
     * position is still best.
     */
    //const int vn_cumulative_position_break_threshold = 20;

    /*! \brief The penalty assigned to changing strings.
     */
    const int vn_string_change = 1;

    /*! \brief The penalty assigned to a Q-shift (using the "wrong" finger for
     * a note)
     */
    const int vn_q_shift_penalty = 5;

    /*! \brief The penalty assigned to a T-move (transferring a finger between
     * strings on successive notes, higher string to lower string)
     */
    const int vn_t_move_penalty = 10;


    /*! \brief The penalty assigned to a traditional layover.
     */
    const int vn_layover_penalty = 2;
    
    /*! \brief The penalty assigned to an O-move (transferring a finger between
     * strings on successive notes, lower string to higher string)
     *
     * An O-move is a sort of sliding layover, so is easier than a T-move.
     */
    const int vn_o_move_penalty = 10;

    /*! \brief The penalty assigned to an A-move (transferring a finger between
     * notes on successive notes, same string, but <b>without</b> a slur effect)
     */
    const int vn_a_move_penalty = 10;

    /*! \brief The penalty assigned to an awkward position change fingering.
     */
    const int vn_bad_pos_change_penalty = 10;
    
    /*!
     * Chunk generation
     * 
     * \todo Parts of this could (and should) be refactored into the superclass
     */
    Chunk VNAlgorithm::generateChunk(ConstNoteIterator cni,
                                        const FingerPosition &start_p,
                                        const FretPos &start_fp,
                                        const Fingering& force_first,
                                        const Note *lead_in)
    {
#ifdef EXTRA_DEBUG
        qDebug("VNAlgorithm::generateChunk");
        qDebug("Starting Note = %s" , (*cni).dbgDump().c_str());
        qDebug("Starting LH Position: %d", start_p);
        qDebug("Starting Fretboard Position: string=%d fret=%d", start_fp.strg, start_fp.fret);
        if (lead_in != 0) {
            qDebug("Lead-in note = %s", lead_in->dbgDump().c_str());
        }
#endif  
        Chunk c;
        c.setPosition(start_p);

        /*
         * Create the first note
         */
         /*! \todo copy in time
         */

        Note n((*cni).noteNum());
        n.setDuration((*cni).duration());
        n.addAnnotation((*cni).annotation());

        FretNum f = start_fp.fret;

        /*
         * Determine which finger is being used for this note.
         */
        FingerNum finger = handmodel_->getFinger(f, start_p);

        /*
         * Nasty hack to enable the starting finger of a chunk to be mandated.
         * This should be handled higher up.
         */
        if ((force_first.finger != NoFingerDefined) && (force_first.finger != finger)) {
            c.addCost(100000); /* Make sure this chunk isn't chosen!! */
            return c;
        }

        Q_ASSERT(finger != NoFingerDefined);

        n.setFinger(finger);
        n.setFret(f);
        n.setString(start_fp.strg);

        Fingering current_fingering;
        Fingering init_fingering = n.fingering();
        /*
         * First, inherent fingering penalties (stretch, weak finger)
         */
        c.addCost(handmodel_->cost(init_fingering, start_p));
        if (handmodel_->isStretch(init_fingering.fret, start_p)) {
            n.addAnnotation(ANNO_STRETCH);
        }

        /*
         * Position change.
         */
        if (lead_in != 0) {
            current_fingering = lead_in->fingering();

            /*
             * Penalties that accrue from awkward ways to change
             * position
             */
            if (init_fingering.finger == current_fingering.finger) {
                if (init_fingering.strg == current_fingering.strg) {
                    /*
                     * Slide is a nice way to change position
                     */
                    n.addAnnotation(HINT_GLISS);
                }
                else if (init_fingering.strg > current_fingering.strg) {
                    n.addAnnotation(ANNO_LAYOVER);
                    /*
                     * Although this is a layover, don't just add the
                     * layover penalty. You really don't want to use a layover
                     * to change position.
                     */
                    c.addCost(vn_bad_pos_change_penalty);
                    c.addCost(20);
                }
                else {
                    n.addAnnotation(ANNO_BADCHANGE);
                    c.addCost(vn_bad_pos_change_penalty);
                    c.addCost(20);
                }
            }
            else if (init_fingering.finger > current_fingering.finger) {
                /*
                 * Changing from e.g. first to fourth, so we want to be
                 * going up the fretboard
                 */
                if (init_fingering.fret > current_fingering.fret) {
                    /*
                     * This is ok (possible extra stretch between fingers)
                     * as long as the stretch isn't too much 
                     *
                     * \todo Should call into HandModel here
                     */
                    if (((init_fingering.fret - current_fingering.fret)
                            - (init_fingering.finger - current_fingering.finger))
                        > 3) { // TODO magic number
                        n.addAnnotation(ANNO_BADSTRETCH);
                        c.addCost(vn_bad_pos_change_penalty);
                    }
                    else if (
                        (((init_fingering.fret - current_fingering.fret)
                            - (init_fingering.finger - current_fingering.finger))
                        > 2) // TODO magic number
                        && init_fingering.finger == LittleFinger
                        ) {
                        n.addAnnotation(ANNO_BADSTRETCH);
                        c.addCost(vn_bad_pos_change_penalty);
                    }
                }
                else if (init_fingering.strg == current_fingering.strg) {
                    init_fingering.finger = current_fingering.finger;
                    n.setFingering(init_fingering);
                    n.addAnnotation(HINT_GLISS);
                }
                else {
                    n.addAnnotation(ANNO_BADCHANGE);
                    c.addCost(vn_bad_pos_change_penalty);
                }
            }
            else /* init_fingering.finger < current_fingering.finger */ {
                /*
                 * Changing from e.g. fourth to first, so we want to be
                 * going down the fretboard
                 */
                if (init_fingering.fret < current_fingering.fret) {
                    /*
                     * This is ok (possible extra stretch between fingers)
                     * as long as the stretch isn't too much 
                     */
                    /*! \todo Should call into HandModel here
                     */
                }
                else if (init_fingering.strg == current_fingering.strg) {
                    init_fingering.finger = current_fingering.finger;
                    n.setFingering(init_fingering);
                    n.addAnnotation(HINT_GLISS);
                }
                else {
                    n.addAnnotation(ANNO_BADCHANGE);
                    c.addCost(vn_bad_pos_change_penalty);
                }
            }
        }

        c.addNote(n);

        /*
         * Now to start iterating through the note list, and see how far we get.
         */
        current_fingering = n.fingering();
        
        ++cni;
        while ((*cni).noteNum() != -1) {

            if ((*cni).noteNum() == 0) {
                /* Rest */
                Note n;
                n.setDuration((*cni).duration());

                c.addNote(n);

                /*
                 * ...and onto the next note
                 */
                ++cni;
                continue;
            }

            Fingering cf = (*cni).fingering();
            
            if ((*cni).hasBreakHint()) {
#ifdef EXTRA_DEBUG
                qDebug("Position break hint after %d notes with score %d", c.length(), c.cost());
#endif
                /*
                 * Return what we have so far
                 */
                return c;
            }

            if ((*cni).hasGlissHint()) {
                /*
                 * Forced gliss
                 */
                cf.finger = current_fingering.finger;
                cf.strg = current_fingering.strg;
            }

            Fingering fingeringtry;
            int lowest_cost = 10000000L;

            /*
             * For each possible place on the fretboard...
             */
            FretPosList fpcandidates = instrument_->candidates(*cni);
            for (FretPosList::const_iterator fp = fpcandidates.begin();
                    fp != fpcandidates.end();
                    ++fp)
            {
                /*
                 * Has the string been mandated?
                 */
                if ((cf.strg != NotDefined) && (cf.strg != (*fp).strg)) {
#ifdef EXTRA_DEBUG
                    qDebug("Auto-string overridden by input hint");
#endif
                    continue;
                }
                
                /*
                 * For each possible LH position....
                 */
                FingerPositionList pcandidates = candidates(*fp);
                for (FingerPositionList::const_iterator p = pcandidates.begin();
                        p != pcandidates.end();
                        ++p)
                {
                    /*
                     * We are only interested if we can avoid shifting position.
                     */
                    if ((*p) == start_p) {
                        Fingering this_fingering;
                        this_fingering.fret = (*fp).fret;
                        this_fingering.strg = (*fp).strg;

                        /*
                         * Work out finger. Has it been mandated?
                         */
                        if ((this_fingering.fret == current_fingering.fret) 
                            && (this_fingering.strg == current_fingering.strg)
                        ) {
                            /*
                             * For repeated notes, maintain the current finger
                             * even if it's a "wrong" one (Q-shift)
                             */
                            this_fingering.finger = current_fingering.finger;
                        }
                        else {
                            this_fingering.finger = handmodel_->getFinger(this_fingering.fret, start_p);
                        }
                        if (cf.finger != NoFingerDefined && cf.finger != this_fingering.finger) {
#ifdef EXTRA_DEBUG
                            qDebug("Auto-fingering overridden by input hint");
#endif
                            continue;
                        }

                        int this_cost = 0;
                        /*
                         * This is where we assign costs based on finger/string moves.
                         */
                        if ((this_fingering.finger == current_fingering.finger) 
                                && (this_fingering.strg < current_fingering.strg)
                                && (!handmodel_->isStretch(this_fingering.fret, start_p))) {
                            this_fingering.addAnnotation(ANNO_QSHIFT);
                            /*
                             * Try a substitution
                             */
                            switch (this_fingering.finger) {
                            case FirstFinger:
                                this_fingering.finger = MiddleFinger;
                                break;
                                
                            case MiddleFinger:
                                this_fingering.finger = RingFinger;
                                break;
                                
                            case RingFinger:
                                this_fingering.finger = LittleFinger;
                                break;
                                
                            case LittleFinger:
                                this_fingering.finger = RingFinger;
                                break;

                            default:
                                /*
                                 * Hmm, shouldn't worry about open strings really.
                                 * Or at least, treat them as a special case.
                                 */
                                break;
                                
                            }
                            this_cost += vn_q_shift_penalty;
                        }


                        this_cost += handmodel_->cost(this_fingering, start_p);
                        if (handmodel_->isStretch(this_fingering.fret, start_p)) {
                            this_fingering.addAnnotation(ANNO_STRETCH);
                        }

                        if (this_fingering.strg != current_fingering.strg) {
                            this_cost += vn_string_change;

                            if (this_fingering.finger == current_fingering.finger) {
                                if (this_fingering.strg < current_fingering.strg) {
                                    /*
                                     * Same finger, lower string (T-move)
                                     */
                                    this_fingering.addAnnotation(ANNO_TMOVE);
                                    this_cost += vn_t_move_penalty;
                                }
                                else if (this_fingering.fret == current_fingering.fret) {
                                    /*
                                     * Same finger/fret, higher string (layover)
                                     */
                                    this_fingering.addAnnotation(ANNO_LAYOVER);
                                    this_cost += vn_layover_penalty;
                                }
                                else /*(this_string > current_string)*/ {
                                    /*
                                     * Same finger, higher string (O-move)
                                     */
                                    this_fingering.addAnnotation(ANNO_OMOVE);
                                    this_cost += vn_o_move_penalty;
                                }
                            }
                        }
                        else if (this_fingering.finger == current_fingering.finger) {
                            if (this_fingering.fret != current_fingering.fret) {
                                /*
                                 * Same finger/string, different fret (A-move)
                                 */
                                this_fingering.addAnnotation(ANNO_AMOVE);
                                this_cost += vn_a_move_penalty;
                            }
                        }
                        
                        
                        /*
                         * Do we have a new best one?
                         */
                        if (this_cost < lowest_cost) {
                            lowest_cost = this_cost;
                            fingeringtry = this_fingering;
                        }
                    }
                }
            }

            /*
             * Is the price too high?
             */
            if (fingeringtry.fret == NotDefined) {
#ifdef SOME_DEBUG
                qDebug("Have to break position, after %d notes with score %d", c.length(), c.cost());
#endif
                /*
                 * Return what we have so far
                 */
                return c;
            }
            else if (lowest_cost > vn_position_break_threshold) {
#ifdef SOME_DEBUG
                qDebug("Next note possible but at cost %d", lowest_cost);
                qDebug("Have to break position, after %d notes with score %d", c.length(), c.cost());
#endif
                /*
                 * Return what we have so far
                 */
                return c;
            }
            else /* if (fingeringtry.fret != NotDefined)*/ {
                Note n((*cni).noteNum());
                n.setDuration((*cni).duration());
                n.setFingering(fingeringtry);
                current_fingering = fingeringtry;
                
#ifdef EXTRA_DEBUG
                qDebug(">>> %s", n.dbgDump().c_str());
                qDebug("c += %d", lowest_cost);
#endif

                c.addNote(n);
                c.addCost(lowest_cost);
                c.addCost(vn_note_bonus);
            }

            /*
             * ...and onto the next note
             */
            ++cni;
        }

        /*
         * Complete success - we have reached the end of the notelist.
         */
#ifdef SOME_DEBUG
        qDebug("Got to the end of the note list with score=%d", c.cost());
        dbgDumpNoteList(c.noteList());
#endif        
        return c;
    }
}
