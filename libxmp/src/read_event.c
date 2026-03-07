/* Extended Module Player
<<<<<<< HEAD
 * Copyright (C) 1996-2021 Claudio Matsuoka and Hipolito Carraro Jr
=======
 * Copyright (C) 1996-2026 Claudio Matsuoka and Hipolito Carraro Jr
>>>>>>> db7344ebf (abc)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "common.h"
#include "player.h"
#include "effects.h"
#include "virtual.h"
#include "period.h"
<<<<<<< HEAD

#ifndef LIBXMP_CORE_PLAYER
#include "med_extras.h"
=======
#include "rng.h"

#ifndef LIBXMP_CORE_PLAYER
#include "med_extras.h"
#include "loaders/med.h"
>>>>>>> db7344ebf (abc)
#endif


static struct xmp_subinstrument *get_subinstrument(struct context_data *ctx,
						   int ins, int key)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_instrument *instrument;

	if (IS_VALID_INSTRUMENT(ins)) {
		instrument = &mod->xxi[ins];
		if (IS_VALID_NOTE(key)) {
			int mapped = instrument->map[key].ins;
			if (mapped != 0xff && mapped >= 0 && mapped < instrument->nsm)
			  	return &instrument->sub[mapped];
		} else {
			if (mod->xxi[ins].nsm > 0) {
				return &instrument->sub[0];
			}
		}
	}

	return NULL;
}

static void reset_envelopes(struct context_data *ctx, struct channel_data *xc)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	if (!IS_VALID_INSTRUMENT(xc->ins))
		return;

	RESET_NOTE(NOTE_ENV_END);

	xc->v_idx = -1;
	xc->p_idx = -1;
	xc->f_idx = -1;
}

#ifndef LIBXMP_CORE_DISABLE_IT

static void reset_envelope_volume(struct context_data *ctx,
				struct channel_data *xc)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	if (!IS_VALID_INSTRUMENT(xc->ins))
		return;

	RESET_NOTE(NOTE_ENV_END);

	xc->v_idx = -1;
}

static void reset_envelopes_carry(struct context_data *ctx,
				struct channel_data *xc)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_instrument *xxi;

	if (!IS_VALID_INSTRUMENT(xc->ins))
		return;

 	RESET_NOTE(NOTE_ENV_END);

	xxi = libxmp_get_instrument(ctx, xc->ins);

	/* Reset envelope positions */
	if (~xxi->aei.flg & XMP_ENVELOPE_CARRY) {
		xc->v_idx = -1;
	}
	if (~xxi->pei.flg & XMP_ENVELOPE_CARRY) {
		xc->p_idx = -1;
	}
	if (~xxi->fei.flg & XMP_ENVELOPE_CARRY) {
		xc->f_idx = -1;
	}
}

#endif

static void set_effect_defaults(struct context_data *ctx, int note,
				struct xmp_subinstrument *sub,
				struct channel_data *xc, int is_toneporta)
{
	struct module_data *m = &ctx->m;

	if (sub != NULL && note >= 0) {
<<<<<<< HEAD
#ifndef LIBXMP_CORE_DISABLE_IT
		struct xmp_module *mod = &m->mod;
		struct smix_data *smix = &ctx->smix;
		struct xmp_instrument *xxi;

		if (xc->ins >= mod->ins) {
			xxi = &smix->xxi[xc->ins - mod->ins];
		} else {
			xxi = &mod->xxi[xc->ins];
		}
#endif

=======
>>>>>>> db7344ebf (abc)
		if (!HAS_QUIRK(QUIRK_PROTRACK)) {
			xc->finetune = sub->fin;
		}
		xc->gvl = sub->gvl;

#ifndef LIBXMP_CORE_DISABLE_IT
		if (sub->ifc & 0x80) {
			xc->filter.cutoff = (sub->ifc - 0x80) * 2;
<<<<<<< HEAD
		} else if (~xxi->fei.flg & XMP_ENVELOPE_FLT) {
			xc->filter.cutoff = 0xff;
=======
>>>>>>> db7344ebf (abc)
		}
		xc->filter.envelope = 0x100;

		if (sub->ifr & 0x80) {
			xc->filter.resonance = (sub->ifr - 0x80) * 2;
<<<<<<< HEAD
		} /* else {
			xc->filter.resonance = 0;
		} */
=======
		}

		/* IT: on a new note without toneporta, allow a computed cutoff
		 * of 127 with resonance 0 to disable the filter. */
		xc->filter.can_disable = !is_toneporta;
>>>>>>> db7344ebf (abc)
#endif

		/* TODO: should probably expand the LFO period size instead
		 * of reducing the vibrato rate precision here.
		 */
		libxmp_lfo_set_depth(&xc->insvib.lfo, sub->vde);
		libxmp_lfo_set_rate(&xc->insvib.lfo, (sub->vra + 2) >> 2);
		libxmp_lfo_set_waveform(&xc->insvib.lfo, sub->vwf);
		xc->insvib.sweep = sub->vsw;

		libxmp_lfo_set_phase(&xc->vibrato.lfo, 0);
		libxmp_lfo_set_phase(&xc->tremolo.lfo, 0);
	}

	xc->delay = 0;
	xc->tremor.up = xc->tremor.down = 0;

	/* Reset arpeggio */
	xc->arpeggio.val[0] = 0;
	xc->arpeggio.count = 0;
	xc->arpeggio.size = 1;
<<<<<<< HEAD
=======

	/* Reset toneporta--each libxmp_process_fx may add to the rate. */
	if (is_toneporta) {
		xc->porta.slide = 0;
	}
}

static void set_channel_volume(struct channel_data *xc, int vol)
{
	if (vol >= 0) {
		xc->volume = vol;
		SET(NEW_VOL);
	}
}

static void set_channel_pan(struct channel_data *xc, int pan)
{
	/* TODO: LIQ supports surround for default panning, unclear if it works.
	 * TODO: Imago Orpheus only temporarily sets channel panning for
	 *       instrument numbers with no note. */
	if (pan >= 0) {
		xc->pan.val = pan;
		xc->pan.surround = 0;
	}
>>>>>>> db7344ebf (abc)
}

/* From OpenMPT PortaTarget.mod:
 * "A new note (with no portamento command next to it) does not reset the
 *  portamento target. That is, if a previous portamento has not finished yet,
 *  calling 3xx or 5xx after the new note will slide it towards the old target.
 *  Once the portamento target period is reached, the target is reset. This
 *  means that if the period is modified by another slide (e.g. 1xx or 2xx),
 *  a following 3xx will not slide back to the original target."
 */
static void set_period(struct context_data *ctx, int note,
				struct xmp_subinstrument *sub,
				struct channel_data *xc, int is_toneporta)
{
	struct module_data *m = &ctx->m;

<<<<<<< HEAD
	if (sub != NULL && note >= 0) {
=======
	/* TODO: blocking period updates on whether or not the event has a
	 * valid instrument seems suspicious, but almost every format uses
	 * this. Only allow Protracker to update without it for now. */
	if (sub == NULL && !HAS_QUIRK(QUIRK_PROTRACK))
		return;

	if (note >= 0) {
>>>>>>> db7344ebf (abc)
		double per = libxmp_note_to_period(ctx, note, xc->finetune,
							xc->per_adj);

		if (!HAS_QUIRK(QUIRK_PROTRACK) || (note > 0 && is_toneporta)) {
			xc->porta.target = per;
		}

		if (xc->period < 1 || !is_toneporta) {
			xc->period = per;
		}
	}
}

/* From OpenMPT Porta-Pickup.xm:
 * "An instrument number should not reset the current portamento target. The
 *  portamento target is valid until a new target is specified by combining a
 *  note and a portamento effect."
 */
<<<<<<< HEAD
static void set_period_ft2(struct context_data *ctx, int note,
				struct xmp_subinstrument *sub,
				struct channel_data *xc, int is_toneporta)
{
	if (note > 0 && is_toneporta) {
		xc->porta.target = libxmp_note_to_period(ctx, note, xc->finetune,
=======
static void set_period_ft2(struct context_data *ctx, int key, int note,
				struct xmp_subinstrument *sub,
				struct channel_data *xc, int is_toneporta)
{
	if (IS_VALID_NOTE(key - 1) && is_toneporta) {
		/* Toneporta target updates even for invalid instruments, using
		 * the default transpose +0 (ft2_invalid_porta_target.xm). */
		int n = key - 1;
		if (sub != NULL) {
			n += ctx->m.mod.xxi[xc->ins].map[key - 1].xpo;
			n += sub->xpo;
		}
		xc->porta.target = libxmp_note_to_period(ctx, n, xc->finetune,
>>>>>>> db7344ebf (abc)
								xc->per_adj);
	}
	if (sub != NULL && note >= 0) {
		if (xc->period < 1 || !is_toneporta) {
			xc->period = libxmp_note_to_period(ctx, note, xc->finetune,
								xc->per_adj);
		}
	}
}


#ifndef LIBXMP_CORE_PLAYER
#define IS_SFX_PITCH(x) ((x) == FX_PITCH_ADD || (x) == FX_PITCH_SUB)
#define IS_TONEPORTA(x) ((x) == FX_TONEPORTA || (x) == FX_TONE_VSLIDE \
		|| (x) == FX_PER_TPORTA || (x) == FX_ULT_TPORTA \
		|| (x) == FX_FAR_TPORTA)
#else
#define IS_TONEPORTA(x) ((x) == FX_TONEPORTA || (x) == FX_TONE_VSLIDE)
#endif

<<<<<<< HEAD
#define set_patch(ctx,chn,ins,smp,note) \
	libxmp_virt_setpatch(ctx, chn, ins, smp, note, 0, 0, 0)

static int read_event_mod(struct context_data *ctx, struct xmp_event *e, int chn)
=======
#define IS_MOD_RETRIG(x,p) ((x) == FX_EXTENDED && MSN(p) == EX_RETRIG && LSN(p) != 0)

#define set_patch(ctx,chn,ins,smp,note) \
	libxmp_virt_setpatch(ctx, chn, ins, smp, note, 0, 0, 0, 0)

static int read_event_mod(struct context_data *ctx, const struct xmp_event *e, int chn)
>>>>>>> db7344ebf (abc)
{
	struct player_data *p = &ctx->p;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct channel_data *xc = &p->xc_data[chn];
	int note;
<<<<<<< HEAD
	struct xmp_subinstrument *sub;
	int new_invalid_ins = 0;
	int is_toneporta;
	int use_ins_vol;
=======
	struct xmp_subinstrument *sub = NULL;
	int new_invalid_ins = 0;
	int new_swap_ins = 0;
	int is_toneporta;
	int is_retrig;
>>>>>>> db7344ebf (abc)

	xc->flags = 0;
	note = -1;
	is_toneporta = 0;
<<<<<<< HEAD
	use_ins_vol = 0;
=======
	is_retrig = 0;
>>>>>>> db7344ebf (abc)

	if (IS_TONEPORTA(e->fxt) || IS_TONEPORTA(e->f2t)) {
		is_toneporta = 1;
	}
<<<<<<< HEAD

	/* Check instrument */

	if (e->ins) {
		int ins = e->ins - 1;
		use_ins_vol = 1;
		SET(NEW_INS);
		xc->fadeout = 0x10000;	/* for painlace.mod pat 0 ch 3 echo */
=======
	if (IS_MOD_RETRIG(e->fxt, e->fxp) || IS_MOD_RETRIG(e->f2t, e->f2p)) {
		is_retrig = 1;
	}

	/* Check instrument */

	if (IS_VALID_NOTE(e->note - 1) && !is_toneporta) {
		xc->key = e->note - 1;
	}
	if (e->ins) {
		int ins = e->ins - 1;
		SET(NEW_INS);
		xc->fadeout = 0x10000;	/* for painlace.mod pat 0 ch 3 echo */
		/* TODO: FunkTracker: instruments probably don't reset
		 * effects, investigate: fnk_note_vslide_cancel.fnk */
>>>>>>> db7344ebf (abc)
		xc->per_flags = 0;
		xc->offset.val = 0;
		RESET_NOTE(NOTE_RELEASE|NOTE_FADEOUT);

		if (IS_VALID_INSTRUMENT(ins)) {
<<<<<<< HEAD
			sub = get_subinstrument(ctx, ins, e->note - 1);

			if (is_toneporta) {
				/* Get new instrument volume */
				if (sub != NULL) {
					/* Dennis Lindroos: instrument volume
					 * is not used on split channels
					 */
					if (!xc->split) {
						xc->volume = sub->vol;
					}
					use_ins_vol = 0;
				}
			} else {
				xc->ins = ins;
				xc->ins_fade = mod->xxi[ins].rls;

				if (sub != NULL) {
					if (HAS_QUIRK(QUIRK_PROTRACK)) {
						xc->finetune = sub->fin;
					}
				}
			}
		} else {
			new_invalid_ins = 1;
			libxmp_virt_resetchannel(ctx, chn);
=======
			sub = get_subinstrument(ctx, ins, xc->key);

			if (sub != NULL) {
				new_swap_ins = 1;

				/* Finetune is always loaded, but only applies
				 * when the period is updated by a note/porta
				 * (OpenMPT finetune.mod, PortaSwapPT.mod). */
				if (HAS_QUIRK(QUIRK_PROTRACK)) {
					xc->finetune = sub->fin;
					xc->ins = ins;
				}

				/* Dennis Lindroos: instrument volume
				 * is not used on split channels
				 */
				if (!xc->split && e->note != XMP_KEY_OFF) {
					set_channel_volume(xc, sub->vol);
					set_channel_pan(xc, sub->pan);
				}
			}

			if (!is_toneporta) {
				xc->ins = ins;
				xc->ins_fade = mod->xxi[ins].rls;
			}
		} else {
			new_invalid_ins = 1;

			/* Invalid instruments do not reset the channel in
			 * Protracker; instead, they set the current sample
			 * to the invalid sample, which stops the current
			 * sample at the end of its loop.
			 *
			 * OpenMPT PTInstrSwap.mod: uses a null sample to pause
			 * a looping sample, plays several on a channel with no note.
			 *
			 * OpenMPT PTSwapEmpty.mod: repeatedly pauses and
			 * restarts a sample using a null sample.
			 */
			if (!HAS_QUIRK(QUIRK_PROTRACK) || is_retrig) {
				libxmp_virt_resetchannel(ctx, chn);
			} else {
				libxmp_virt_queuepatch(ctx, chn, -1, -1, 0);
			}
>>>>>>> db7344ebf (abc)
		}
	}

	/* Check note */

	if (e->note) {
		SET(NEW_NOTE);
<<<<<<< HEAD

		if (e->note == XMP_KEY_OFF) {
			SET_NOTE(NOTE_RELEASE);
			use_ins_vol = 0;
		} else if (!is_toneporta && IS_VALID_NOTE(e->note - 1)) {
			xc->key = e->note - 1;
=======
		/* FunkTracker - new notes cancel persistent volume slide.
		 * Farandole Composer notes are always paired with volume,
		 * so this doesn't notably affect it. */
		RESET_PER(VOL_SLIDE);

		if (e->note == XMP_KEY_OFF) {
			SET_NOTE(NOTE_RELEASE);
		} else if (!is_toneporta && IS_VALID_NOTE(e->note - 1)) {
>>>>>>> db7344ebf (abc)
			RESET_NOTE(NOTE_END);

			sub = get_subinstrument(ctx, xc->ins, xc->key);

<<<<<<< HEAD
			if (!new_invalid_ins && sub != NULL) {
=======
			if (sub != NULL) {
>>>>>>> db7344ebf (abc)
				int transp = mod->xxi[xc->ins].map[xc->key].xpo;
				int smp;

				note = xc->key + sub->xpo + transp;
				smp = sub->sid;

<<<<<<< HEAD
				if (!IS_VALID_SAMPLE(smp)) {
=======
				if (new_invalid_ins || !IS_VALID_SAMPLE(smp)) {
>>>>>>> db7344ebf (abc)
					smp = -1;
				}

				if (smp >= 0 && smp < mod->smp) {
					set_patch(ctx, chn, xc->ins, smp, note);
<<<<<<< HEAD
=======
					new_swap_ins = 0;
>>>>>>> db7344ebf (abc)
					xc->smp = smp;
				}
			} else {
				xc->flags = 0;
<<<<<<< HEAD
				use_ins_vol = 0;
			}
		}
	}

=======
				note = xc->key;
			}
		}

		if (note >= 0) {
			xc->note = note;
			SET_NOTE(NOTE_SET);
		}
	}

	/* Protracker 1/2 sample swap occurs when a sample number is
	 * encountered without a note or with a note and toneporta. The new
	 * instrument is switched to when the current sample reaches its loop
	 * end. A valid note must have been played in this channel before.
	 *
	 * Empty samples can also be set, which stops the sample at the end
	 * of its loop (see above).
	 */
	if (new_swap_ins && sub && HAS_QUIRK(QUIRK_PROTRACK) && TEST_NOTE(NOTE_SET)) {
		libxmp_virt_queuepatch(ctx, chn, e->ins - 1, sub->sid, xc->note);
		xc->smp = sub->sid;
	}

	/* sub is now the currently playing subinstrument, which may not be
	 * related to e->ins if there is active toneporta! */
>>>>>>> db7344ebf (abc)
	sub = get_subinstrument(ctx, xc->ins, xc->key);

	set_effect_defaults(ctx, note, sub, xc, is_toneporta);
	if (e->ins && sub != NULL) {
		reset_envelopes(ctx, xc);
	}

	/* Process new volume */
<<<<<<< HEAD
	if (e->vol) {
		xc->volume = e->vol - 1;
		SET(NEW_VOL);
		RESET_PER(VOL_SLIDE); /* FIXME: should this be for FAR only? */
=======
	set_channel_volume(xc, e->vol - 1);
	if (e->vol) {
		/* Farandole Composer - volume resets slide to volume. */
		RESET_PER(VOL_SLIDE);
>>>>>>> db7344ebf (abc)
	}

	/* Secondary effect handled first */
	libxmp_process_fx(ctx, xc, chn, e, 1);
	libxmp_process_fx(ctx, xc, chn, e, 0);

#ifndef LIBXMP_CORE_PLAYER
	if (IS_SFX_PITCH(e->fxt)) {
 		xc->period = libxmp_note_to_period(ctx, note, xc->finetune,
                                			xc->per_adj);
	} else
#endif
	{
		set_period(ctx, note, sub, xc, is_toneporta);
	}

	if (sub == NULL) {
		return 0;
	}

<<<<<<< HEAD
	if (note >= 0) {
		xc->note = note;
		libxmp_virt_voicepos(ctx, chn, xc->offset.val);
=======
	if (note >= 0 && !new_invalid_ins) {
		libxmp_virt_voicepos(ctx, chn, xc->offset.val);
	} else if (new_swap_ins && is_retrig && HAS_QUIRK(QUIRK_PROTRACK)) {
		/* Protracker: an instrument number with no note and retrigger
		 * triggers the new sample on tick 0. Other effects that set
		 * RETRIG should not. (OpenMPT InstrSwapRetrigger.mod) */
		libxmp_virt_voicepos(ctx, chn, 0);
>>>>>>> db7344ebf (abc)
	}

	if (TEST(OFFSET)) {
		if (HAS_QUIRK(QUIRK_PROTRACK) || p->flags & XMP_FLAGS_FX9BUG) {
			xc->offset.val += xc->offset.val2;
		}
		RESET(OFFSET);
	}

<<<<<<< HEAD
	if (use_ins_vol && !TEST(NEW_VOL) && !xc->split) {
		xc->volume = sub->vol;
	}

=======
>>>>>>> db7344ebf (abc)
	return 0;
}

static int sustain_check(struct xmp_envelope *env, int idx)
{
	return (env &&
		(env->flg & XMP_ENVELOPE_ON) &&
		(env->flg & XMP_ENVELOPE_SUS) &&
		(~env->flg & XMP_ENVELOPE_LOOP) &&
		idx == env->data[env->sus << 1]);
}

<<<<<<< HEAD
static int read_event_ft2(struct context_data *ctx, struct xmp_event *e, int chn)
=======
static int read_event_ft2(struct context_data *ctx, const struct xmp_event *e, int chn)
>>>>>>> db7344ebf (abc)
{
	struct player_data *p = &ctx->p;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct channel_data *xc = &p->xc_data[chn];
<<<<<<< HEAD
	int note, key, ins;
	struct xmp_subinstrument *sub;
	int new_invalid_ins;
	int is_toneporta;
	int use_ins_vol;
	int k00 = 0;
	struct xmp_event ev;

	/* From the OpenMPT DelayCombination.xm test case:
         * "Naturally, Fasttracker 2 ignores notes next to an out-of-range
=======
	int note, key;
	struct xmp_subinstrument *sub;
	int is_toneporta;
	int is_delayed;
	struct xmp_event ev;

	/* From the OpenMPT DelayCombination.xm test case:
	 * "Naturally, Fasttracker 2 ignores notes next to an out-of-range
>>>>>>> db7344ebf (abc)
	 *  note delay. However, to check whether the delay is out of range,
	 *  it is simply compared against the current song speed, not taking
	 *  any pattern delays into account."
	 */
	if (p->frame >= p->speed) {
		return 0;
	}

	memcpy(&ev, e, sizeof (struct xmp_event));

<<<<<<< HEAD
	/* From OpenMPT TremorReset.xm test case:
	 * "Even if a tremor effect muted the sample on a previous row, volume
	 *  commands should be able to override this effect."
	 */
	if (ev.vol) {
		xc->tremor.count &= ~0x80;
	}

	xc->flags = 0;
	note = -1;
	key = ev.note;
	ins = ev.ins;
	new_invalid_ins = 0;
	is_toneporta = 0;
	use_ins_vol = 0;

	/* From the OpenMPT key_off.xm test case:
	 * "Key off at tick 0 (K00) is very dodgy command. If there is a note
	 *  next to it, the note is ignored. If there is a volume column
	 *  command or instrument next to it and the current instrument has
	 *  no volume envelope, the note is faded out instead of being cut."
	 */
	if (ev.fxt == FX_KEYOFF && ev.fxp == 0) {
		k00 = 1;
		key = 0;

		if (ins || ev.vol || ev.f2t) {
			if (IS_VALID_INSTRUMENT(xc->ins) &&
			    ~mod->xxi[xc->ins].aei.flg & XMP_ENVELOPE_ON) {
				SET_NOTE(NOTE_FADEOUT);
				ev.fxt = 0;
			}
		}
=======
	xc->flags = 0;
	note = -1;
	key = ev.note;
	is_toneporta = 0;
	is_delayed = 0;

	/* Delay has a few bizarre hacks that need to be supported. */
	if (ev.fxt == FX_EXTENDED && MSN(ev.fxp) == EX_DELAY && LSN(ev.fxp)) {
		/* No note + delay -> note memory (ft2_delay_note_memory.xm).
		 * Combined with ins# memory, effectively causes a retrigger. */
		key = key ? key : xc->key_memory ? xc->key_memory : xc->key + 1;

		/* Key off + no ins# + delay + volume column pan -> ignore pan
		 * (OpenMPT PanOff.xm) (ft2_delay_volume_column.xm).
		 */
		if (HAS_QUIRK(QUIRK_FT2BUGS) && key == XMP_KEY_OFF &&
		    !ev.ins && ev.f2t == FX_SETPAN) {
			ev.f2t = ev.f2p = 0;
		}
		is_delayed = 1;
>>>>>>> db7344ebf (abc)
	}

	if (IS_TONEPORTA(ev.fxt) || IS_TONEPORTA(ev.f2t)) {
		is_toneporta = 1;
<<<<<<< HEAD
	}

	/* Check instrument */

	/* Ignore invalid instruments. The last instrument, invalid or
	 * not, is preserved in channel data (see read_event() below).
	 * Fixes stray delayed notes in forgotten_city.xm.
	 */
	if (ins > 0 && !IS_VALID_INSTRUMENT(ins - 1)) {
		ins = 0;
	}

	/* FT2: Retrieve old instrument volume */
	if (ins) {
		if (key == 0 || key >= XMP_KEY_OFF) {
			/* Previous instrument */
			sub = get_subinstrument(ctx, xc->ins, xc->key);

			/* No note */
			if (sub != NULL) {
				int pan = mod->xxc[chn].pan - 128;
				xc->volume = sub->vol;

				if (!HAS_QUIRK(QUIRK_FTMOD)) {
					xc->pan.val = pan + ((sub->pan - 128) *
						(128 - abs(pan))) / 128 + 128;
				}

				xc->ins_fade = mod->xxi[xc->ins].rls;
				SET(NEW_VOL);
			}
		}
	}

	/* Do this regardless if the instrument is invalid or not -- unless
	 * XM keyoff is used. Fixes xyce-dans_la_rue.xm chn 0 patterns 0E/0F and
	 * chn 10 patterns 0D/0E, see https://github.com/libxmp/libxmp/issues/152
	 * for details.
         */
	if (ev.ins && key != XMP_KEY_FADE) {
		SET(NEW_INS);
		use_ins_vol = 1;
		xc->per_flags = 0;

		RESET_NOTE(NOTE_RELEASE|NOTE_SUSEXIT);
		if (!k00) {
			RESET_NOTE(NOTE_FADEOUT);
		}

		xc->fadeout = 0x10000;

		if (IS_VALID_INSTRUMENT(ins - 1)) {
			if (!is_toneporta)
				xc->ins = ins - 1;
		} else {
			new_invalid_ins = 1;

			/* If no note is set FT2 doesn't cut on invalid
			 * instruments (it keeps playing the previous one).
			 * If a note is set it cuts the current sample.
			 */
			xc->flags = 0;

			if (is_toneporta) {
				key = 0;
			}
		}

		xc->tremor.count = 0x20;
	}

	/* Check note */
	if (ins) {
		if (key > 0 && key < XMP_KEY_OFF) {
			/* Retrieve volume when we have note */

			/* and only if we have instrument, otherwise we're in
			 * case 1: new note and no instrument
			 */

			/* Current instrument */
			sub = get_subinstrument(ctx, xc->ins, key - 1);
			if (sub != NULL) {
				int pan = mod->xxc[chn].pan - 128;
				xc->volume = sub->vol;

				if (!HAS_QUIRK(QUIRK_FTMOD)) {
					xc->pan.val = pan + ((sub->pan - 128) *
						(128 - abs(pan))) / 128 + 128;
				}

				xc->ins_fade = mod->xxi[xc->ins].rls;
			} else {
				xc->volume = 0;
			}
			SET(NEW_VOL);
		}
	}

	if (key) {
		SET(NEW_NOTE);

		if (key == XMP_KEY_OFF) {
			int env_on = 0;
			int vol_set = ev.vol != 0 || ev.fxt == FX_VOLSET;
			int delay_fx = ev.fxt == FX_EXTENDED && ev.fxp == 0xd0;
			struct xmp_envelope *env = NULL;

			/* OpenMPT NoteOffVolume.xm:
			 * "If an instrument has no volume envelope, a note-off
			 *  command should cut the sample completely - unless
			 *  there is a volume command next it. This applies to
			 *  both volume commands (volume and effect column)."
			 *
			 * ...and unless we have a keyoff+delay without setting
			 * an instrument. See OffDelay.xm.
			 */
			if (IS_VALID_INSTRUMENT(xc->ins)) {
				env = &mod->xxi[xc->ins].aei;
				if (env->flg & XMP_ENVELOPE_ON) {
					env_on = 1;
				}
			}

			if (env_on || (!vol_set && (!ev.ins || !delay_fx))) {
				if (sustain_check(env, xc->v_idx)) {
					/* See OpenMPT EnvOff.xm. In certain
					 * cases a release event is effective
					 * only in the next frame
					 */
					SET_NOTE(NOTE_SUSEXIT);
				} else {
					SET_NOTE(NOTE_RELEASE);
				}
				use_ins_vol = 0;
			} else {
				SET_NOTE(NOTE_FADEOUT);
			}

			/* See OpenMPT keyoff+instr.xm, pattern 2 row 0x40 */
			if (env_on && ev.fxt == FX_EXTENDED &&
			    (ev.fxp >> 4) == EX_DELAY) {
				/* See OpenMPT OffDelay.xm test case */
				if ((ev.fxp & 0xf) != 0) {
					RESET_NOTE(NOTE_RELEASE|NOTE_SUSEXIT);
				}
			}
		} else if (key == XMP_KEY_FADE) {
			/* Handle keyoff + instrument case (NoteOff2.xm) */
			SET_NOTE(NOTE_FADEOUT);
		} else if (is_toneporta) {
			/* set key to 0 so we can have the tone portamento from
			 * the original note (see funky_stars.xm pos 5 ch 9)
			 */
			key = 0;

			/* And do the same if there's no keyoff (see comic
			 * bakery remix.xm pos 1 ch 3)
			 */
		}

		if (ev.ins == 0 && !IS_VALID_INSTRUMENT(xc->old_ins - 1)) {
			new_invalid_ins = 1;
		}

		if (new_invalid_ins) {
			libxmp_virt_resetchannel(ctx, chn);
		}
	}


	/* Check note range -- from the OpenMPT test NoteLimit.xm:
	 * "I think one of the first things Fasttracker 2 does when parsing a
	 *  pattern cell is calculating the “real” note (i.e. pattern note +
	 *  sample transpose), and if this “real” note falls out of its note
	 *  range, it is ignored completely (wiped from its internal channel
	 *  memory). The instrument number next it, however, is not affected
	 *  and remains in the memory."
	 */
	sub = NULL;
	if (IS_VALID_NOTE(key - 1)) {
		int k = key - 1;
		sub = get_subinstrument(ctx, xc->ins, k);
		if (!new_invalid_ins && sub != NULL) {
			int transp = mod->xxi[xc->ins].map[k].xpo;
			int k2 = k + sub->xpo + transp;
			if (k2 < 12 || k2 > 130) {
				key = 0;
				RESET(NEW_NOTE);
			}
		}
	}

	if (IS_VALID_NOTE(key - 1)) {
		xc->key = --key;
		xc->fadeout = 0x10000;
		RESET_NOTE(NOTE_END);

		if (sub != NULL) {
			if (~mod->xxi[xc->ins].aei.flg & XMP_ENVELOPE_ON) {
				RESET_NOTE(NOTE_RELEASE|NOTE_FADEOUT);
			}
		}

		if (!new_invalid_ins && sub != NULL) {
			int transp = mod->xxi[xc->ins].map[key].xpo;
			int smp;

			note = key + sub->xpo + transp;
			smp = sub->sid;

			if (!IS_VALID_SAMPLE(smp)) {
				smp = -1;
			}

			if (smp >= 0 && smp < mod->smp) {
				set_patch(ctx, chn, xc->ins, smp, note);
				xc->smp = smp;
			}
		} else {
			xc->flags = 0;
			use_ins_vol = 0;
		}
	}

	sub = get_subinstrument(ctx, xc->ins, xc->key);

	set_effect_defaults(ctx, note, sub, xc, is_toneporta);

	if (ins && sub != NULL && !k00) {
		/* Reset envelopes on new instrument, see olympic.xm pos 10
		 * But make sure we have an instrument set, see Letting go
		 * pos 4 chn 20
		 */
		reset_envelopes(ctx, xc);
	}

	/* Process new volume */
	if (ev.vol) {
		xc->volume = ev.vol - 1;
		SET(NEW_VOL);
		if (TEST_NOTE(NOTE_END)) {	/* m5v-nine.xm */
			xc->fadeout = 0x10000;	/* OpenMPT NoteOff.xm */
			RESET_NOTE(NOTE_RELEASE|NOTE_FADEOUT);
		}
	}
=======
		/* Mx + 3xx/5xy applies toneporta for both commands, but 3xx
		 * uses the rate from the volume slot (ft2_double_toneporta.xm,
		 * OpenMPT TonePortamentoMemory.xm). */
		if (HAS_QUIRK(QUIRK_FT2BUGS) &&
		    ev.fxt == FX_TONEPORTA && IS_TONEPORTA(ev.f2t)) {
			ev.fxp = 0;
		}
	}

	/* FT2 deletes K00 and, if there is no volume fx toneporta, overwrites
	 * the note with keyoff (ft2_k00_is_note_off.xm, OpenMPT key_off.xm,
	 * ft2_envelope_reset.xm). */
	if (ev.fxt == FX_KEYOFF && ev.fxp == 0) {
		ev.fxt = 0;
		if (!is_toneporta) {
			key = XMP_KEY_OFF;
		}
	}

	/* Check instrument
	 *
	 * Only update instrument/sample on new valid note + no toneporta/K00.
	 * Lamb/forgotten city.xm relies heavily on quirks here.
	 */
	if (ev.ins) {
		SET(NEW_INS);
		xc->per_flags = 0; /* For posterity; not used by XM */
	}
	if (IS_VALID_NOTE(key - 1) && !is_toneporta) {
		struct xmp_instrument *xxi = NULL;
		/* Note w/o instrument loads the last referenced instrument. */
		int ins = ev.ins ? ev.ins : xc->old_ins;
		int smp = -1;
		int n = key - 1;

		/* Updates on note + !toneporta + !K00 and is unaffected by
		 * out-of-range transposition checks (ft2_note_range.xm). */
		xc->key_memory = key;

		/* Unused instruments have fade 0x80, no envelopes, no vibrato.
		 * Unused samples have volume 0, pan 0x80, transpose 0, no data.
		 * libxmp represents unused/invalid instruments/samples as -1.
		 * (test_player_ft2_note_noins_after_invalid_ins)
		 * (test_player_ft2_note_off_after_invalid_ins)
		 */
		if (IS_VALID_INSTRUMENT(ins - 1)) {
			xxi = &mod->xxi[ins - 1];
			sub = get_subinstrument(ctx, ins - 1, key - 1);
			if (sub) {
				n += xxi->map[key - 1].xpo;
				n += sub->xpo;
				smp = sub->sid;
			}
		}
		/* TODO: out-of-range notes update envelopes, no ins.# req. */

		/* Fade update requires ins.# (ft2_instrument_fade_update.xm).
		 * Out-of-range notes update (ft2_note_range_instrument_fade.xm). */
		if (ev.ins) {
			xc->ins_fade = xxi ? xxi->rls :
					0x80 /* FT2 default */ << 1 /* conv */;
		}

		/* Valid notes update the instrument, sample, key, and note.
		 * Note B-(-1) updates key/instrument/sample, but not the note.
		 * Notes >A#9, <B-(-1) act as if the key does not exist at all.
		 * (ft2_note_range.xm, OpenMPT NoteLimit.xm/NoteLimit2.xm)
		 */
		if (n >= FT2_NOTE_BN1 && n <= FT2_NOTE_AS9) {
			if (n >= FT2_NOTE_C0) {
				xc->note = n;
			}
			xc->key = key - 1;
			xc->ins = IS_VALID_INSTRUMENT(ins - 1) ? ins - 1 : -1;
			xc->smp = IS_VALID_SAMPLE(smp) ? smp : -1;
		} else {
			key = 0;
		}
	}
	/* Get the new instrument. If the instrument/key wasn't updated, this
	 * is equivalent to FT2 retaining the previous instrument/sample. */
	sub = get_subinstrument(ctx, xc->ins, xc->key);

	/* Check note
	 *
	 * Do not send a new note for toneporta (Quazar/funky stars.xm pos 5
	 * ch 9, Mark Birch/comic bakery remix.xm pos 1 ch 3).
	 */
	if (key) {
		SET(NEW_NOTE);
	}
	if (IS_VALID_NOTE(key - 1) && !is_toneporta) {
		RESET_NOTE(NOTE_END);

		/* Send note even if the current sample is invalid.
		 * Playing with an active invalid sample cuts the channel:
		 * invalid instrument, valid instrument with invalid subins, or
		 * zero-length sample (test_player_ft2_invalid_ins_defaults).
		 */
		set_patch(ctx, chn, xc->ins, xc->smp, xc->note);
		note = xc->note;
	}

	/* Check key off/envelopes */

	if (key == XMP_KEY_OFF) {
		struct xmp_envelope *env = NULL;

		if (IS_VALID_INSTRUMENT(xc->ins)) {
			env = &mod->xxi[xc->ins].aei;
		}

		if (env != NULL && (env->flg & XMP_ENVELOPE_ON)) {
			if (sustain_check(env, xc->v_idx)) {
				/* See OpenMPT EnvOff.xm. In certain
				 * cases a release event is effective
				 * only in the next frame
				 */
				SET_NOTE(NOTE_SUSEXIT);
			} else {
				SET_NOTE(NOTE_RELEASE);
			}
		} else {
			/* No volume envelope -> cut volume to 0
			 * (ft2_note_off_fade.xm, OpenMPT NoteOffVolume.xm). */
			set_channel_volume(xc, 0);
		}

		/* Keyoff always begins fadeout (ft2_note_off_fade.xm). */
		SET_NOTE(NOTE_FADEOUT);
	}
	if ((ev.ins && key != XMP_KEY_OFF) || is_delayed) {
		/* Reset release/fadeout for instrument numbers with no keyoff/K00
		 * (xyce-dans_la_rue.xm chn 0 pat. 0E/0F, chn 10 pat. 0D/0E;
		 * ft2_k00_defaults.xm; ft2_note_off_sustain.xm), and on
		 * delayed rows (ft2_delay_envelope_*.xm, ft2_note_off_fade.xm,
		 * OpenMPT NoteOff.xm). Other cases like note w/o ins# don't
		 * reset fadeout (Cave Story - Last Battle.xm pos 11 chn 2,
		 * ft2_note_no_fadeout_reset.xm).
		 */
		xc->fadeout = 0x10000;
		RESET_NOTE(NOTE_FADEOUT);
		RESET_NOTE(NOTE_RELEASE|NOTE_SUSEXIT);

		if (sub != NULL) {
			/* Only reset envelopes with a valid active instrument
			 * (ft2_envelope_invalid_ins.xm)
			 * (Ghidorah/olympic dance.xm pos 10)
			 * (Jeroen Tel/letting go.xm pos 4 chn 20).
			 */
			reset_envelopes(ctx, xc);
		}

		/* Tremor count resets with fadeout (ft2_tremor_reset.xm). */
		xc->tremor.count = TREMOR_SUPPRESS;
	}

	/* TODO: this function needs checking, probably split. */
	set_effect_defaults(ctx, note, sub, xc, is_toneporta);

	if (ev.ins) {
		/* Any ins.#: use active sample for defaults. Invalid samples
		 * have volume 0 panning 0x80 (ft2_invalid_ins_defaults.xm).
		 * Works on lines with K00 (ft2_k00_defaults.xm).
		 */
		set_channel_volume(xc, sub ? sub->vol : 0);
		set_channel_pan(xc, sub ? sub->pan : 0x80);
	}

	/* Process new volume */
	set_channel_volume(xc, ev.vol - 1);
>>>>>>> db7344ebf (abc)

	/* FT2: always reset sample offset */
	xc->offset.val = 0;

	/* Secondary effect handled first */
	libxmp_process_fx(ctx, xc, chn, &ev, 1);
	libxmp_process_fx(ctx, xc, chn, &ev, 0);
<<<<<<< HEAD
	set_period_ft2(ctx, note, sub, xc, is_toneporta);

	if (sub == NULL) {
		return 0;
	}

	if (note >= 0) {
		xc->note = note;

		/* From the OpenMPT test cases (3xx-no-old-samp.xm):
		 * "An offset effect that points beyond the sample end should
		 *  stop playback on this channel."
		 *
		 * ... except in Skale Tracker (and possibly others), so make this a
		 *  FastTracker2 quirk. See Armada Tanks game.it (actually an XM).
		 *  Reported by Vladislav Suschikh.
		 */
		if (HAS_QUIRK(QUIRK_FT2BUGS) && xc->offset.val >= mod->xxs[sub->sid].len) {
			libxmp_virt_resetchannel(ctx, chn);
		} else {

			/* (From Decibelter - Cosmic 'Wegian Mamas.xm p04 ch7)
			 * We retrigger the sample only if we have a new note
			 * without tone portamento, otherwise we won't play
			 * sweeps and loops correctly.
			 */
			libxmp_virt_voicepos(ctx, chn, xc->offset.val);
		}
	}

	if (use_ins_vol && !TEST(NEW_VOL)) {
		xc->volume = sub->vol;
=======
	set_period_ft2(ctx, key, note, sub, xc, is_toneporta);
	/* TODO: Modplug, MT2, rstST process some FX every tick (affects toneporta memory). */

	if (TEST(NEW_VOL)) {
		/* Tremor is reset by ins# without keyoff or by delay rows.
		 * Other events that set volume (volume column/Cxx, keyoff+ins#)
		 * also temporarily override tremor, but don't reset it.
		 * (Tremor likely just overwrites the channel volume in FT2.)
		 * (ft2_tremor_reset.xm, OpenMPT TremorRecover.xm)
		 */
		xc->tremor.count |= TREMOR_SUPPRESS;
	}

	if (note >= 0) {
		/* Sample offset requires valid note + 9xx + !toneporta
		 * (Decibelter/Cosmic 'Wegian Mamas.xm pattern 4 channel 8).
		 * In FT2, memory is set ONLY in these cases, and offsets past
		 * the end of the sample cut. (Skale Tracker is different, so
		 * limit this to QUIRK_FT2BUGS; reported by Vladislav Suschikh.)
		 * (ft2_offset_memory.xm, OpenMPT 3xx-no-old-samp.xm/porta-offset.xm)
		 */
		if (HAS_QUIRK(QUIRK_FT2BUGS) && TEST(OFFSET)) {
			xc->offset.memory = (xc->offset.val & 0xff00) >> 8;

			if (!IS_VALID_SAMPLE(xc->smp) ||
			    xc->offset.val >= mod->xxs[xc->smp].len) {
				libxmp_virt_resetchannel(ctx, chn);
			}
		}
		libxmp_virt_voicepos(ctx, chn, xc->offset.val);
>>>>>>> db7344ebf (abc)
	}

	return 0;
}

<<<<<<< HEAD
static int read_event_st3(struct context_data *ctx, struct xmp_event *e, int chn)
=======
static int read_event_st3(struct context_data *ctx, const struct xmp_event *e, int chn)
>>>>>>> db7344ebf (abc)
{
	struct player_data *p = &ctx->p;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct channel_data *xc = &p->xc_data[chn];
	int note;
	struct xmp_subinstrument *sub;
	int not_same_ins;
	int is_toneporta;
<<<<<<< HEAD
	int use_ins_vol;
=======
>>>>>>> db7344ebf (abc)

	xc->flags = 0;
	note = -1;
	not_same_ins = 0;
	is_toneporta = 0;
<<<<<<< HEAD
	use_ins_vol = 0;
=======
>>>>>>> db7344ebf (abc)

	if (IS_TONEPORTA(e->fxt) || IS_TONEPORTA(e->f2t)) {
		is_toneporta = 1;
	}

	if (libxmp_virt_mapchannel(ctx, chn) < 0 && xc->ins != e->ins - 1) {
		is_toneporta = 0;
	}

	/* Check instrument */

<<<<<<< HEAD
	if (e->ins) {
		int ins = e->ins - 1;
		SET(NEW_INS);
		use_ins_vol = 1;
=======
	if (IS_VALID_NOTE(e->note - 1) && !is_toneporta) {
		xc->key = e->note - 1;
	}
	if (e->ins) {
		int ins = e->ins - 1;
		SET(NEW_INS);
>>>>>>> db7344ebf (abc)
		xc->fadeout = 0x10000;
		xc->per_flags = 0;
		xc->offset.val = 0;
		RESET_NOTE(NOTE_RELEASE|NOTE_FADEOUT);

		if (IS_VALID_INSTRUMENT(ins)) {
<<<<<<< HEAD
			/* valid ins */
=======
>>>>>>> db7344ebf (abc)
			if (xc->ins != ins) {
				not_same_ins = 1;
				if (!is_toneporta) {
					xc->ins = ins;
					xc->ins_fade = mod->xxi[ins].rls;
<<<<<<< HEAD
				} else {
					/* Get new instrument volume */
					sub = get_subinstrument(ctx, ins, e->note - 1);
					if (sub != NULL) {
						xc->volume = sub->vol;
						use_ins_vol = 0;
					}
				}
			}
		} else {
			/* invalid ins */

			/* Ignore invalid instruments */
			xc->flags = 0;
			use_ins_vol = 0;
=======
				}
			}

			/* Get new instrument volume */
			sub = get_subinstrument(ctx, ins, xc->key);
			if (sub != NULL && e->note != XMP_KEY_OFF) {
				set_channel_volume(xc, sub->vol);
				set_channel_pan(xc, sub->pan);
			}
		} else {
			/* Ignore invalid instruments */
			xc->flags = 0;
>>>>>>> db7344ebf (abc)
		}
	}

	/* Check note */

	if (e->note) {
		SET(NEW_NOTE);

		if (e->note == XMP_KEY_OFF) {
			SET_NOTE(NOTE_RELEASE);
<<<<<<< HEAD
			use_ins_vol = 0;
=======
>>>>>>> db7344ebf (abc)
		} else if (is_toneporta) {
			/* Always retrig in tone portamento: Fix portamento in
			 * 7spirits.s3m, mod.Biomechanoid
			 */
			if (not_same_ins) {
				xc->offset.val = 0;
			}
		} else if (IS_VALID_NOTE(e->note - 1)) {
<<<<<<< HEAD
			xc->key = e->note - 1;
=======
>>>>>>> db7344ebf (abc)
			RESET_NOTE(NOTE_END);

			sub = get_subinstrument(ctx, xc->ins, xc->key);

			if (sub != NULL) {
				int transp = mod->xxi[xc->ins].map[xc->key].xpo;
				int smp;

				note = xc->key + sub->xpo + transp;
				smp = sub->sid;

				if (!IS_VALID_SAMPLE(smp)) {
					smp = -1;
				}

				if (smp >= 0 && smp < mod->smp) {
					set_patch(ctx, chn, xc->ins, smp, note);
					xc->smp = smp;
				}
			} else {
				xc->flags = 0;
<<<<<<< HEAD
				use_ins_vol = 0;
=======
>>>>>>> db7344ebf (abc)
			}
		}
	}

<<<<<<< HEAD
=======
	/* sub is now the currently playing subinstrument, which may not be
	 * related to e->ins if there is active toneporta! */
>>>>>>> db7344ebf (abc)
	sub = get_subinstrument(ctx, xc->ins, xc->key);

	set_effect_defaults(ctx, note, sub, xc, is_toneporta);
	if (e->ins && sub != NULL) {
		reset_envelopes(ctx, xc);
	}

	/* Process new volume */
<<<<<<< HEAD
	if (e->vol) {
		xc->volume = e->vol - 1;
		SET(NEW_VOL);
	}
=======
	set_channel_volume(xc, e->vol - 1);
>>>>>>> db7344ebf (abc)

	/* Secondary effect handled first */
	libxmp_process_fx(ctx, xc, chn, e, 1);
	libxmp_process_fx(ctx, xc, chn, e, 0);
	set_period(ctx, note, sub, xc, is_toneporta);
<<<<<<< HEAD
=======
	/* TODO: Orpheus processes some FX every tick (affects toneporta memory);
	 * toneporta is additive only if both values are the same (otherwise,
	 * use the value from slot 2). */
>>>>>>> db7344ebf (abc)

	if (sub == NULL) {
		return 0;
	}

	if (note >= 0) {
		xc->note = note;
		libxmp_virt_voicepos(ctx, chn, xc->offset.val);
	}

<<<<<<< HEAD
	if (use_ins_vol && !TEST(NEW_VOL)) {
		xc->volume = sub->vol;
	}

=======
>>>>>>> db7344ebf (abc)
	if (HAS_QUIRK(QUIRK_ST3BUGS) && TEST(NEW_VOL)) {
		xc->volume = xc->volume * p->gvol / m->volbase;
	}

	return 0;
}

#ifndef LIBXMP_CORE_DISABLE_IT

static inline void copy_channel(struct player_data *p, int to, int from)
{
	if (to > 0 && to != from) {
		memcpy(&p->xc_data[to], &p->xc_data[from],
					sizeof (struct channel_data));
	}
}

<<<<<<< HEAD
static inline int has_note_event(struct xmp_event *e)
{
	return (e->note && e->note <= XMP_MAX_KEYS);
}

=======
>>>>>>> db7344ebf (abc)
static int check_fadeout(struct context_data *ctx, struct channel_data *xc, int ins)
{
	struct xmp_instrument *xxi = libxmp_get_instrument(ctx, ins);

	if (xxi == NULL) {
		return 1;
	}

	return (~xxi->aei.flg & XMP_ENVELOPE_ON ||
		~xxi->aei.flg & XMP_ENVELOPE_CARRY ||
		xc->ins_fade == 0 ||
		xc->fadeout <= xc->ins_fade);
}

static int check_invalid_sample(struct context_data *ctx, int ins, int key)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	if (ins < mod->ins) {
		int smp = mod->xxi[ins].map[key].ins;
		if (smp == 0xff || smp >= mod->smp) {
			return 1;
		};
	}

	return 0;
}

static void fix_period(struct context_data *ctx, int chn, struct xmp_subinstrument *sub)
{
	if (sub->nna == XMP_INST_NNA_CONT) {
		struct player_data *p = &ctx->p;
		struct channel_data *xc = &p->xc_data[chn];
		struct xmp_instrument *xxi = libxmp_get_instrument(ctx, xc->ins);

		xc->period = libxmp_note_to_period(ctx, xc->key + sub->xpo +
			xxi->map[xc->key_porta].xpo, xc->finetune, xc->per_adj);
	}
}

static int is_same_sid(struct context_data *ctx, int chn, int ins, int key)
{
	struct player_data *p = &ctx->p;
	struct channel_data *xc = &p->xc_data[chn];
	struct xmp_subinstrument *s1, *s2;

	s1 = get_subinstrument(ctx, ins, key);
	s2 = get_subinstrument(ctx, xc->ins, xc->key);

	return (s1 && s2 && s1->sid == s2->sid);
}

<<<<<<< HEAD
static int read_event_it(struct context_data *ctx, struct xmp_event *e, int chn)
=======
static int read_event_it(struct context_data *ctx, const struct xmp_event *e, int chn)
>>>>>>> db7344ebf (abc)
{
	struct player_data *p = &ctx->p;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct channel_data *xc = &p->xc_data[chn];
	int note, key;
	struct xmp_subinstrument *sub;
	int not_same_ins, not_same_smp;
	int new_invalid_ins;
	int is_toneporta, is_release;
	int candidate_ins;
	int reset_env;
<<<<<<< HEAD
	int use_ins_vol;
	int sample_mode;
	int toneporta_offset;
	int disabled_toneporta;
=======
	int reset_susloop;
	int use_ins_vol;
	int sample_mode;
	int toneporta_offset;
>>>>>>> db7344ebf (abc)
	int retrig_ins;
	struct xmp_event ev;

	memcpy(&ev, e, sizeof (struct xmp_event));

	/* Emulate Impulse Tracker "always read instrument" bug */
	if (ev.ins) {
		xc->delayed_ins = 0;
	} else if (ev.note && xc->delayed_ins) {
		ev.ins = xc->delayed_ins;
		xc->delayed_ins = 0;
	}

	xc->flags = 0;
	note = -1;
	key = ev.note;
	not_same_ins = 0;
	not_same_smp = 0;
	new_invalid_ins = 0;
	is_toneporta = 0;
	is_release = 0;
	reset_env = 0;
<<<<<<< HEAD
=======
	reset_susloop = 0;
>>>>>>> db7344ebf (abc)
	use_ins_vol = 0;
	candidate_ins = xc->ins;
	sample_mode = !HAS_QUIRK(QUIRK_VIRTUAL);
	toneporta_offset = 0;
<<<<<<< HEAD
	disabled_toneporta = 0;
=======
>>>>>>> db7344ebf (abc)
	retrig_ins = 0;

	/* Keyoff + instrument retrigs current instrument in old fx mode */
	if (HAS_QUIRK(QUIRK_ITOLDFX)) {
		if (ev.note == XMP_KEY_OFF && IS_VALID_INSTRUMENT(ev.ins -1)) {
			retrig_ins = 1;
		}
	}

	/* Notes with unmapped instruments are ignored */
	if (ev.ins) {
<<<<<<< HEAD
		if (ev.ins <= mod->ins && has_note_event(&ev)) {
=======
		if (ev.ins <= mod->ins && IS_VALID_NOTE(ev.note - 1)) {
>>>>>>> db7344ebf (abc)
			int ins = ev.ins - 1;
			if (check_invalid_sample(ctx, ins, ev.note - 1)) {
				candidate_ins = ins;
				memset(&ev, 0, sizeof (ev));
			}
		}
	} else {
<<<<<<< HEAD
		if (has_note_event(&ev)) {
=======
		if (IS_VALID_NOTE(ev.note - 1)) {
>>>>>>> db7344ebf (abc)
			int ins = xc->old_ins - 1;
			if (!IS_VALID_INSTRUMENT(ins)) {
				new_invalid_ins = 1;
			} else if (check_invalid_sample(ctx, ins, ev.note - 1)) {
				memset(&ev, 0, sizeof (ev));
			}
		}
	}

	if (IS_TONEPORTA(ev.fxt) || IS_TONEPORTA(ev.f2t)) {
		is_toneporta = 1;
	}

<<<<<<< HEAD
	if (TEST_NOTE(NOTE_RELEASE | NOTE_FADEOUT)) {
=======
	if (TEST_NOTE(NOTE_ENV_RELEASE | NOTE_FADEOUT)) {
>>>>>>> db7344ebf (abc)
		is_release = 1;
	}

	if (xc->period <= 0 || TEST_NOTE(NOTE_END)) {
		is_toneporta = 0;
	}

	/* Off-Porta.it */
	if (is_toneporta && ev.fxt == FX_OFFSET) {
<<<<<<< HEAD
		disabled_toneporta = 1;
		is_toneporta = 0;
 		if (!HAS_QUIRK(QUIRK_PRENV)) {
			toneporta_offset = 1;
=======
		toneporta_offset = 1;
 		if (!HAS_QUIRK(QUIRK_PRENV)) {
>>>>>>> db7344ebf (abc)
			RESET_NOTE(NOTE_ENV_END);
		}
	}

	/* Check instrument */

	if (ev.ins) {
		int ins = ev.ins - 1;
		int set_new_ins = 1;

		/* portamento_after_keyoff.it test case */
		if (is_release && !key) {
			if (is_toneporta) {
				if (HAS_QUIRK(QUIRK_PRENV) || TEST_NOTE(NOTE_SET)) {
					is_toneporta = 0;
					reset_envelopes_carry(ctx, xc);
				}
			} else {
				/* fixes OpenMPT wnoteoff.it */
				reset_envelopes_carry(ctx, xc);
			}
		}

		if (is_toneporta && xc->ins == ins) {
			if (!HAS_QUIRK(QUIRK_PRENV)) {
				if (is_same_sid(ctx, chn, ins, key - 1)) {
					/* same instrument and same sample */
					set_new_ins = !is_release;
				} else {
					/* same instrument, different sample */
					not_same_ins = 1; /* need this too */
					not_same_smp = 1;
				}
			}
		}

		if (set_new_ins) {
			SET(NEW_INS);
<<<<<<< HEAD
			use_ins_vol = 1;
			reset_env = 1;
		}
=======
			reset_env = 1;
		}
		/* Sample default volume is always enabled if a valid sample
		 * is provided (Atomic Playboy, default_volume.it). */
		use_ins_vol = 1;
>>>>>>> db7344ebf (abc)
		xc->per_flags = 0;

		if (IS_VALID_INSTRUMENT(ins)) {
			/* valid ins */

			/* See OpenMPT StoppedInstrSwap.it for cut case */
			if (!key && !TEST_NOTE(NOTE_KEY_CUT)) {
				/* Retrig in new ins in sample mode */
				if (sample_mode && TEST_NOTE(NOTE_END)) {
					libxmp_virt_voicepos(ctx, chn, 0);
				}

				/* IT: Reset note for every new != ins */
				if (xc->ins == ins) {
					SET(NEW_INS);
					use_ins_vol = 1;
				} else {
					key = xc->key + 1;
				}

				RESET_NOTE(NOTE_SET);
			}

			if (xc->ins != ins && (!is_toneporta || !HAS_QUIRK(QUIRK_PRENV))) {
				candidate_ins = ins;

				if (!is_same_sid(ctx, chn, ins, key - 1)) {
					not_same_ins = 1;
					if (is_toneporta) {
						/* Get new instrument volume */
						sub = get_subinstrument(ctx, ins, key);
						if (sub != NULL) {
							xc->volume = sub->vol;
							use_ins_vol = 0;
						}
					}
				}
			}
		} else {
<<<<<<< HEAD
			/* In sample mode invalid ins cut previous ins */
=======
			/* In sample mode invalid instruments cut the current
			 * note (OpenMPT SampleNumberChange.it).
			 * TODO: portamento_sustain.it order 3 row 19: when
			 * sample release is set, this isn't always done? */
>>>>>>> db7344ebf (abc)
			if (sample_mode) {
				xc->volume = 0;
			}

			/* Ignore invalid instruments */
			new_invalid_ins = 1;
			xc->flags = 0;
			use_ins_vol = 0;
		}
	}

	/* Check note */

<<<<<<< HEAD
	if (key && !new_invalid_ins) {
=======
	if (key) {
>>>>>>> db7344ebf (abc)
		SET(NEW_NOTE);
		SET_NOTE(NOTE_SET);

		if (key == XMP_KEY_FADE) {
			SET_NOTE(NOTE_FADEOUT);
			reset_env = 0;
<<<<<<< HEAD
=======
			reset_susloop = 0;
>>>>>>> db7344ebf (abc)
			use_ins_vol = 0;
		} else if (key == XMP_KEY_CUT) {
			SET_NOTE(NOTE_END | NOTE_CUT | NOTE_KEY_CUT);
			xc->period = 0;
			libxmp_virt_resetchannel(ctx, chn);
		} else if (key == XMP_KEY_OFF) {
			struct xmp_envelope *env = NULL;
			if (IS_VALID_INSTRUMENT(xc->ins)) {
				env = &mod->xxi[xc->ins].aei;
			}
			if (sustain_check(env, xc->v_idx)) {
				SET_NOTE(NOTE_SUSEXIT);
			} else {
				SET_NOTE(NOTE_RELEASE);
			}
			SET(KEY_OFF);
			/* Use instrument volume if an instrument was explicitly
			 * provided on this row (see OpenMPT NoteOffInstr.it row 4).
			 * However, never reset the envelope (see OpenMPT wnoteoff.it).
			 */
			reset_env = 0;
<<<<<<< HEAD
			if (!ev.ins) {
				use_ins_vol = 0;
			}
		} else {
			/* portamento_after_keyoff.it test case */
			/* also see suburban_streets o13 c45 */
			if (ev.ins || !is_toneporta) {
				if (!disabled_toneporta) {
					reset_env = 1;
				}
=======
			reset_susloop = 0;
			if (!ev.ins) {
				use_ins_vol = 0;
			}
		} else if (!new_invalid_ins) {
			/* Sample sustain release should always carry for tone
			 * portamento, and is not reset unless a note is
			 * present (Atomic Playboy, portamento_sustain.it). */
			/* portamento_after_keyoff.it test case */
			/* also see suburban_streets o13 c45 */
			if (!is_toneporta) {
				reset_env = 1;
				reset_susloop = 1;
>>>>>>> db7344ebf (abc)
			}

			if (is_toneporta) {
				if (not_same_ins || TEST_NOTE(NOTE_END)) {
					SET(NEW_INS);
<<<<<<< HEAD
					RESET_NOTE(NOTE_RELEASE|NOTE_SUSEXIT|NOTE_FADEOUT);
=======
					RESET_NOTE(NOTE_ENV_RELEASE|NOTE_SUSEXIT|NOTE_FADEOUT);
>>>>>>> db7344ebf (abc)
				} else {
					if (IS_VALID_NOTE(key - 1)) {
						xc->key_porta = key - 1;
					}
					key = 0;
				}
			}
		}
	}

<<<<<<< HEAD
=======
	/* TODO: instrument change+porta(+release?) doesn't require a key.
	 * Order 3/row 11 of portamento_sustain.it should change the sample. */
>>>>>>> db7344ebf (abc)
	if (IS_VALID_NOTE(key - 1) && !new_invalid_ins) {
		if (TEST_NOTE(NOTE_CUT)) {
			use_ins_vol = 1;	/* See OpenMPT NoteOffInstr.it */
		}
		xc->key = --key;
		RESET_NOTE(NOTE_END);

		sub = get_subinstrument(ctx, candidate_ins, key);

		if (sub != NULL) {
			int transp = mod->xxi[candidate_ins].map[key].xpo;
			int smp, to;
<<<<<<< HEAD
			int rvv;

=======
			int dct;
			int rvv;

			/* Clear note delay before duplicating channels:
			 * it_note_delay_nna.it */
			xc->delay = 0;

>>>>>>> db7344ebf (abc)
			note = key + sub->xpo + transp;
			smp = sub->sid;
			if (!IS_VALID_SAMPLE(smp)) {
				smp = -1;
			}
<<<<<<< HEAD

			if (not_same_smp) {
				fix_period(ctx, chn, sub);
				libxmp_virt_resetchannel(ctx, chn);
			}
			to = libxmp_virt_setpatch(ctx, chn, candidate_ins, smp,
				note, sub->nna, sub->dct, sub->dca);
=======
			dct = sub->dct;

			if (not_same_smp) {
				fix_period(ctx, chn, sub);
				/* Toneporta, even when not executed, disables
				 * NNA and DCAs for the current note:
				 * portamento_nna_sample.it, gxsmp2.it */
				libxmp_virt_setnna(ctx, chn, XMP_INST_NNA_CUT);
				dct = XMP_INST_DCT_OFF;
			}
			to = libxmp_virt_setpatch(ctx, chn, candidate_ins, smp,
				note, key, sub->nna, dct, sub->dca);
>>>>>>> db7344ebf (abc)

			/* Random value for volume swing */
			rvv = sub->rvv & 0xff;
			if (rvv) {
				CLAMP(rvv, 0, 100);
<<<<<<< HEAD
				xc->rvv = rand() % (rvv + 1);
=======
				xc->rvv = libxmp_get_random(&ctx->rng, rvv + 1);
>>>>>>> db7344ebf (abc)
			} else {
				xc->rvv = 0;
			}

			/* Random value for pan swing */
			rvv = (sub->rvv & 0xff00) >> 8;
			if (rvv) {
				CLAMP(rvv, 0, 64);
<<<<<<< HEAD
				xc->rpv = rand() % (rvv + 1) - (rvv / 2);
=======
				xc->rpv = libxmp_get_random(&ctx->rng, rvv + 1) - (rvv / 2);
>>>>>>> db7344ebf (abc)
			} else {
				xc->rpv = 0;
			}

			if (to < 0)
				return -1;
			if (to != chn) {
				copy_channel(p, to, chn);
				p->xc_data[to].flags = 0;
			}

			if (smp >= 0) {		/* Not sure if needed */
				xc->smp = smp;
			}
		} else {
			xc->flags = 0;
			use_ins_vol = 0;
		}
	}

	/* Do after virtual channel copy */
	if (is_toneporta || retrig_ins) {
		if (HAS_QUIRK(QUIRK_PRENV) && ev.ins) {
			reset_envelopes_carry(ctx, xc);
		}
	}

	if (IS_VALID_INSTRUMENT(candidate_ins)) {
		if (xc->ins != candidate_ins) {
			/* Reset envelopes if instrument changes */
			reset_envelopes(ctx, xc);
		}
		xc->ins = candidate_ins;
		xc->ins_fade = mod->xxi[candidate_ins].rls;
	}

	/* Reset in case of new instrument and the previous envelope has
	 * finished (OpenMPT test EnvReset.it). This must take place after
	 * channel copies in case of NNA (see test/test.it)
	 * Also if we have envelope in carry mode, check fadeout
	 * Also, only reset the volume envelope. (it_fade_env_reset_carry.it)
	 */
	if (ev.ins && TEST_NOTE(NOTE_ENV_END)) {
		if (check_fadeout(ctx, xc, candidate_ins)) {
			reset_envelope_volume(ctx, xc);
		} else {
			reset_env = 0;
		}
	}

	if (reset_env) {
		if (ev.note) {
<<<<<<< HEAD
			RESET_NOTE(NOTE_RELEASE|NOTE_SUSEXIT|NOTE_FADEOUT);
=======
			RESET_NOTE(NOTE_ENV_RELEASE|NOTE_SUSEXIT|NOTE_FADEOUT);
>>>>>>> db7344ebf (abc)
		}
		/* Set after copying to new virtual channel (see ambio.it) */
		xc->fadeout = 0x10000;
	}
<<<<<<< HEAD
=======
	if (reset_susloop && ev.note) {
		RESET_NOTE(NOTE_SAMPLE_RELEASE);
	}
>>>>>>> db7344ebf (abc)

	/* See OpenMPT wnoteoff.it vs noteoff3.it */
	if (retrig_ins && not_same_ins) {
		SET(NEW_INS);
		libxmp_virt_voicepos(ctx, chn, 0);
		xc->fadeout = 0x10000;
		RESET_NOTE(NOTE_RELEASE|NOTE_SUSEXIT|NOTE_FADEOUT);
	}

	sub = get_subinstrument(ctx, xc->ins, xc->key);

	set_effect_defaults(ctx, note, sub, xc, is_toneporta);
	if (sub != NULL) {
		if (note >= 0) {
			/* Reset pan, see OpenMPT PanReset.it */
<<<<<<< HEAD
			if (sub->pan >= 0) {
				xc->pan.val = sub->pan;
				xc->pan.surround = 0;
			}

			if (TEST_NOTE(NOTE_CUT)) {
				reset_envelopes(ctx, xc);
			} else if (!toneporta_offset) {
=======
			set_channel_pan(xc, sub->pan);

			if (TEST_NOTE(NOTE_CUT)) {
				reset_envelopes(ctx, xc);
			} else if (!toneporta_offset || HAS_QUIRK(QUIRK_PRENV)) {
>>>>>>> db7344ebf (abc)
				reset_envelopes_carry(ctx, xc);
			}
			RESET_NOTE(NOTE_CUT);
		}
	}

	/* Process new volume */
	if (ev.vol && (!TEST_NOTE(NOTE_CUT) || ev.ins != 0)) {
		/* Do this even for XMP_KEY_OFF (see OpenMPT NoteOffInstr.it row 4). */
<<<<<<< HEAD
		xc->volume = ev.vol - 1;
		SET(NEW_VOL);
=======
		set_channel_volume(xc, ev.vol - 1);
>>>>>>> db7344ebf (abc)
	}

	/* IT: always reset sample offset */
	xc->offset.val &= ~0xffff;

	/* According to Storlek test 25, Impulse Tracker handles the volume
	 * column effects after the standard effects.
<<<<<<< HEAD
=======
	 * TODO: IT updates toneporta memory on first tick but reloads memory
	 * to use for the rate for both channels every tick afterward.
	 * TODO: Modplug processes some FX every tick (affects toneporta memory).
>>>>>>> db7344ebf (abc)
	 */
	libxmp_process_fx(ctx, xc, chn, &ev, 0);
	libxmp_process_fx(ctx, xc, chn, &ev, 1);
	set_period(ctx, note, sub, xc, is_toneporta);

	if (sub == NULL) {
		return 0;
	}

	if (note >= 0) {
		xc->note = note;
<<<<<<< HEAD
=======
	}
	if (note >= 0 || toneporta_offset) {
>>>>>>> db7344ebf (abc)
		libxmp_virt_voicepos(ctx, chn, xc->offset.val);
	}

	if (use_ins_vol && !TEST(NEW_VOL)) {
		xc->volume = sub->vol;
	}

	return 0;
}

#endif

#ifndef LIBXMP_CORE_PLAYER

<<<<<<< HEAD
static int read_event_med(struct context_data *ctx, struct xmp_event *e, int chn)
=======
static int read_event_med(struct context_data *ctx, const struct xmp_event *e, int chn)
>>>>>>> db7344ebf (abc)
{
	struct player_data *p = &ctx->p;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct channel_data *xc = &p->xc_data[chn];
	int note;
<<<<<<< HEAD
	struct xmp_subinstrument *sub;
	int new_invalid_ins = 0;
	int is_toneporta;
	int use_ins_vol;
	int finetune;
=======
	struct xmp_instrument *xxi;
	struct xmp_subinstrument *sub;
	int new_invalid_ins = 0;
	int is_toneporta;
	int finetune;
	int tracker_version = MED_MODULE_EXTRAS(*m)->tracker_version;
>>>>>>> db7344ebf (abc)

	xc->flags = 0;
	note = -1;
	is_toneporta = 0;
<<<<<<< HEAD
	use_ins_vol = 0;

=======

	/* TODO: 5xy (FX_TONE_VSLIDE) can't initiate toneporta,
	 * only continue an active one. It does not set the target
	 * even on an active toneporta? Instrument sets hold/decay
	 * if 5xy is present. For all intents and purposes, 5xy
	 * should not be setting this, it seems? */
>>>>>>> db7344ebf (abc)
	if (e->fxt == FX_TONEPORTA || e->fxt == FX_TONE_VSLIDE) {
		is_toneporta = 1;
	}

	/* Check instrument */

	if (e->ins && e->note) {
		int ins = e->ins - 1;
<<<<<<< HEAD
		use_ins_vol = 1;
=======
>>>>>>> db7344ebf (abc)
		SET(NEW_INS);
		xc->fadeout = 0x10000;
		xc->offset.val = 0;
		RESET_NOTE(NOTE_RELEASE|NOTE_FADEOUT);

		if (IS_VALID_INSTRUMENT(ins)) {
<<<<<<< HEAD
			if (is_toneporta) {
				/* Get new instrument volume */
				sub = get_subinstrument(ctx, ins, e->note - 1);
				if (sub != NULL) {
					xc->volume = sub->vol;
					use_ins_vol = 0;
				}
			} else {
				xc->ins = ins;
				xc->ins_fade = mod->xxi[ins].rls;
=======
			if (!is_toneporta) {
				xxi = &mod->xxi[ins];
				xc->ins = ins;
				xc->ins_fade = xxi->rls;

				if (HAS_MED_INSTRUMENT_EXTRAS(*xxi)) {
					libxmp_med_set_hold_decay(ctx, xc,
						MED_INSTRUMENT_EXTRAS(*xxi)->hold,
						MED_INSTRUMENT_EXTRAS(*xxi)->decay);
				}
>>>>>>> db7344ebf (abc)
			}
		} else {
			new_invalid_ins = 1;
			libxmp_virt_resetchannel(ctx, chn);
		}

		MED_CHANNEL_EXTRAS(*xc)->arp = 0;
		MED_CHANNEL_EXTRAS(*xc)->aidx = 0;
<<<<<<< HEAD
	} else {
		/* Hold */
		if (e->ins && !e->note) {
			use_ins_vol = 1;
=======
	}
	if (e->ins) {
		/* Hold symbols apply default volume from OctaMED 3.00 onward,
		 * but they do not in older trackers. All events with a note
		 * and an instrument apply default volume.
		 */
		if (IS_VALID_NOTE(e->note - 1) ||
		    tracker_version >= MED_VER_OCTAMED_300) {
			/* Get new instrument volume */
			sub = get_subinstrument(ctx, e->ins - 1, e->note - 1);
			if (sub != NULL) {
				set_channel_volume(xc, sub->vol);
			}
>>>>>>> db7344ebf (abc)
		}
	}

	/* Check note */

	if (e->note) {
		SET(NEW_NOTE);

		if (e->note == XMP_KEY_OFF) {
			SET_NOTE(NOTE_RELEASE);
<<<<<<< HEAD
			use_ins_vol = 0;
=======
>>>>>>> db7344ebf (abc)
		} else if (e->note == XMP_KEY_CUT) {
			SET_NOTE(NOTE_END);
			xc->period = 0;
			libxmp_virt_resetchannel(ctx, chn);
<<<<<<< HEAD
		} else if (!is_toneporta && IS_VALID_INSTRUMENT(xc->ins) && IS_VALID_NOTE(e->note - 1)) {
			struct xmp_instrument *xxi = &mod->xxi[xc->ins];
=======
		} else if (!is_toneporta && IS_VALID_INSTRUMENT(xc->ins) &&
			   IS_VALID_NOTE(e->note - 1)) {
			xxi = &mod->xxi[xc->ins];
>>>>>>> db7344ebf (abc)

			xc->key = e->note - 1;
			RESET_NOTE(NOTE_END);

			xc->per_adj = 0.0;
			if (xxi->nsm > 1 && HAS_MED_INSTRUMENT_EXTRAS(*xxi)) {
				/* synth or iffoct */
				if (MED_INSTRUMENT_EXTRAS(*xxi)->vts == 0 &&
				    MED_INSTRUMENT_EXTRAS(*xxi)->wts == 0) {
					/* iffoct */
					xc->per_adj = 2.0;
				}
			}

			sub = get_subinstrument(ctx, xc->ins, xc->key);

			if (!new_invalid_ins && sub != NULL) {
				int transp = xxi->map[xc->key].xpo;
				int smp;

				note = xc->key + sub->xpo + transp;
				smp = sub->sid;

				if (!IS_VALID_SAMPLE(smp)) {
					smp = -1;
				}

				if (smp >= 0 && smp < mod->smp) {
					set_patch(ctx, chn, xc->ins, smp, note);
					xc->smp = smp;
				}
			} else {
				xc->flags = 0;
<<<<<<< HEAD
				use_ins_vol = 0;
=======
>>>>>>> db7344ebf (abc)
			}
		}
	}

<<<<<<< HEAD
=======
	/* sub is now the currently playing subinstrument, which may not be
	 * related to e->ins if there is active toneporta! */
>>>>>>> db7344ebf (abc)
	sub = get_subinstrument(ctx, xc->ins, xc->key);

	/* Keep effect-set finetune if no instrument set */
	finetune = xc->finetune;
	set_effect_defaults(ctx, note, sub, xc, is_toneporta);
	if (!e->ins) {
		xc->finetune = finetune;
	}

	if (e->ins && sub != NULL) {
		reset_envelopes(ctx, xc);
	}

	/* Process new volume */
<<<<<<< HEAD
	if (e->vol) {
		xc->volume = e->vol - 1;
		SET(NEW_VOL);
	}
=======
	set_channel_volume(xc, e->vol - 1);
>>>>>>> db7344ebf (abc)

	/* Secondary effect handled first */
	libxmp_process_fx(ctx, xc, chn, e, 1);
	libxmp_process_fx(ctx, xc, chn, e, 0);
	set_period(ctx, note, sub, xc, is_toneporta);

<<<<<<< HEAD
=======
	/* Test next line for a hold symbol. Hold is set either by the
	 * instrument or by command 08 Hold/Decay, so test after effects. */
	libxmp_med_check_hold_symbol(ctx, xc, chn);

>>>>>>> db7344ebf (abc)
	if (sub == NULL) {
		return 0;
	}

	if (note >= 0) {
		xc->note = note;
		libxmp_virt_voicepos(ctx, chn, xc->offset.val);
	}

<<<<<<< HEAD
	if (use_ins_vol && !TEST(NEW_VOL)) {
		xc->volume = sub->vol;
	}

=======
>>>>>>> db7344ebf (abc)
	return 0;
}

#endif

<<<<<<< HEAD
static int read_event_smix(struct context_data *ctx, struct xmp_event *e, int chn)
=======
static int read_event_smix(struct context_data *ctx, const struct xmp_event *e, int chn)
>>>>>>> db7344ebf (abc)
{
	struct player_data *p = &ctx->p;
	struct smix_data *smix = &ctx->smix;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct channel_data *xc = &p->xc_data[chn];
	struct xmp_subinstrument *sub;
<<<<<<< HEAD
	int is_smix_ins;
=======
	struct xmp_instrument *xxi;
>>>>>>> db7344ebf (abc)
	int ins, note, transp, smp;

	xc->flags = 0;

	if (!e->ins)
		return 0;

<<<<<<< HEAD
	is_smix_ins = 0;
	ins = e->ins - 1;
	SET(NEW_INS);
	xc->fadeout = 0x10000;
	xc->per_flags = 0;
	xc->offset.val = 0;
	RESET_NOTE(NOTE_RELEASE);

	xc->ins = ins;

	if (ins >= mod->ins && ins < mod->ins + smix->ins) {
		is_smix_ins = 1;
		xc->ins_fade = smix->xxi[xc->ins - mod->ins].rls;
	}
=======
	ins = e->ins - 1;
	SET(NEW_INS);
	xc->per_flags = 0;
	xc->offset.val = 0;
	RESET_NOTE(NOTE_RELEASE|NOTE_FADEOUT);

	xxi = libxmp_get_instrument(ctx, ins);
	if (xxi != NULL) {
		xc->ins_fade = xxi->rls;
	}
	xc->ins = ins;
>>>>>>> db7344ebf (abc)

	SET(NEW_NOTE);

	if (e->note == XMP_KEY_OFF) {
		SET_NOTE(NOTE_RELEASE);
		return 0;
<<<<<<< HEAD
	}

	xc->key = e->note - 1;
	RESET_NOTE(NOTE_END);

	if (is_smix_ins) {
		sub = &smix->xxi[xc->ins - mod->ins].sub[0];
=======
	} else if (e->note == XMP_KEY_FADE) {
		SET_NOTE(NOTE_FADEOUT);
		return 0;
	} else if (e->note == XMP_KEY_CUT) {
		SET_NOTE(NOTE_END);
		xc->period = 0;
		libxmp_virt_resetchannel(ctx, chn);
		return 0;
	}

	xc->key = e->note - 1;
	xc->fadeout = 0x10000;
	RESET_NOTE(NOTE_END);

	if (ins >= mod->ins && ins < mod->ins + smix->ins) {
		sub = &xxi->sub[0];
>>>>>>> db7344ebf (abc)
		if (sub == NULL) {
			return 0;
		}

		note = xc->key + sub->xpo;
		smp = sub->sid;
		if (smix->xxs[smp].len == 0)
			smp = -1;
		if (smp >= 0 && smp < smix->smp) {
			smp += mod->smp;
			set_patch(ctx, chn, xc->ins, smp, note);
			xc->smp = smp;
		}
	} else {
		sub = IS_VALID_NOTE(xc->key) ?
			get_subinstrument(ctx, xc->ins, xc->key) : NULL;
		if (sub == NULL) {
			return 0;
		}
<<<<<<< HEAD
		transp = mod->xxi[xc->ins].map[xc->key].xpo;
=======
		transp = xxi->map[xc->key].xpo;
>>>>>>> db7344ebf (abc)
		note = xc->key + sub->xpo + transp;
		smp = sub->sid;
		if (!IS_VALID_SAMPLE(smp))
			smp = -1;
		if (smp >= 0 && smp < mod->smp) {
			set_patch(ctx, chn, xc->ins, smp, note);
			xc->smp = smp;
		}
	}

	set_effect_defaults(ctx, note, sub, xc, 0);
	set_period(ctx, note, sub, xc, 0);

	if (e->ins) {
		reset_envelopes(ctx, xc);
	}

<<<<<<< HEAD
	xc->volume = e->vol - 1;
=======
	set_channel_volume(xc, e->vol - 1);
>>>>>>> db7344ebf (abc)

	xc->note = note;
	libxmp_virt_voicepos(ctx, chn, xc->offset.val);

	return 0;
}

<<<<<<< HEAD
int libxmp_read_event(struct context_data *ctx, struct xmp_event *e, int chn)
=======
int libxmp_read_event(struct context_data *ctx, const struct xmp_event *e, int chn)
>>>>>>> db7344ebf (abc)
{
	struct player_data *p = &ctx->p;
	struct module_data *m = &ctx->m;
	struct channel_data *xc = &p->xc_data[chn];

	if (e->ins != 0)
		xc->old_ins = e->ins;

	if (TEST_NOTE(NOTE_SAMPLE_END)) {
		SET_NOTE(NOTE_END);
	}

	if (chn >= m->mod.chn) {
		return read_event_smix(ctx, e, chn);
	} else switch (m->read_event_type) {
	case READ_EVENT_MOD:
		return read_event_mod(ctx, e, chn);
	case READ_EVENT_FT2:
		return read_event_ft2(ctx, e, chn);
	case READ_EVENT_ST3:
		return read_event_st3(ctx, e, chn);
#ifndef LIBXMP_CORE_DISABLE_IT
	case READ_EVENT_IT:
		return read_event_it(ctx, e, chn);
#endif
#ifndef LIBXMP_CORE_PLAYER
	case READ_EVENT_MED:
		return read_event_med(ctx, e, chn);
#endif
	default:
		return read_event_mod(ctx, e, chn);
	}
}
