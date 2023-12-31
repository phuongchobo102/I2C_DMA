/**
 * \file            lwbtn.c
 * \brief           Lightweight event system
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwBTN - Lightweight button manager.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v0.0.1
 */
#include <string.h>
#include "lwbtn.h"

#if LWBTN_CFG_GET_STATE_MODE > 2
#error "Invalid LWBTN_GET_STATE_MODE_CALLBACK configuration"
#endif

#define LWBTN_FLAG_ONPRESS_SENT ((uint16_t)0x0001) /*!< Flag indicates that on-press event has been sent */
#define LWBTN_FLAG_MANUAL_STATE                                                                                        \
    ((uint16_t)0x0002) /*!< Flag indicates that user wants to manually set button state.
                                                    Do not call "get_state" function */

#define LWBTN_TIME_DEBOUNCE_GET_MIN(btn)      LWBTN_CFG_TIME_DEBOUNCE
#define LWBTN_TIME_CLICK_GET_PRESSED_MIN(btn) LWBTN_CFG_TIME_CLICK_MIN
#define LWBTN_TIME_CLICK_GET_PRESSED_MAX(btn) LWBTN_CFG_TIME_CLICK_MAX
#define LWBTN_TIME_CLICK_MAX_MULTI(btn)       LWBTN_CFG_TIME_CLICK_MULTI_MAX
#define LWBTN_TIME_KEEPALIVE_PERIOD(btn)      LWBTN_CFG_TIME_KEEPALIVE_PERIOD
#define LWBTN_CLICK_MAX_CONSECUTIVE(btn)      LWBTN_CFG_CLICK_MAX_CONSECUTIVE

/* Get button state */
#if LWBTN_CFG_GET_STATE_MODE == LWBTN_GET_STATE_MODE_CALLBACK
#define LWBTN_BTN_GET_STATE(lwobj, btn) ((lwobj)->get_state_fn((lwobj), (btn)))
#elif LWBTN_CFG_GET_STATE_MODE == LWBTN_GET_STATE_MODE_MANUAL
#define LWBTN_BTN_GET_STATE(lwobj, btn) ((btn)->curr_state)
#elif LWBTN_CFG_GET_STATE_MODE == LWBTN_GET_STATE_MODE_CALLBACK_OR_MANUAL
#define LWBTN_BTN_GET_STATE(lwobj, btn)                                                                                \
    (((btn)->flags & LWBTN_FLAG_MANUAL_STATE)                                                                          \
         ? ((btn)->curr_state)                                                                                         \
         : (((lwobj)->get_state_fn != NULL) ? ((lwobj)->get_state_fn((lwobj), (btn))) : 0))
#endif

/* Default button group instance */
static lwbtn_t lwbtn_default;
#define LWBTN_GET_LWOBJ(in_lwobj) ((in_lwobj) != NULL ? (in_lwobj) : (&lwbtn_default))


void
prv_process_btn(lwbtn_t* lwobj, lwbtn_btn_t* btn, uint32_t mstime);

/**
 * \brief           Process the button information and state
 * 
 * \param[in]       lwobj: LwBTN instance. Set to `NULL` to use default one
 * \param[in]       btn: Button instance to process
 * \param[in]       mstime: Current milliseconds system time
 */
void
prv_process_btn(lwbtn_t* lwobj, lwbtn_btn_t* btn, uint32_t mstime) {
    uint8_t new_state;

    /* Get button state */
    new_state = LWBTN_BTN_GET_STATE(lwobj, btn);

    /*
     * Button state has changed
     */
    if (new_state != btn->old_state) {
        /*
         * Button just became inactive
         *
         * - Handle on-release event
         * - Handle on-click event
         */
        if (!new_state) {
            /*
             * We only need to react if on-press event has even been started.
             *
             * Do nothing if that was not the case
             */
            if (btn->flags & LWBTN_FLAG_ONPRESS_SENT) {
                /* Handle on-release event */
                btn->flags &= ~LWBTN_FLAG_ONPRESS_SENT;
                lwobj->evt_fn(lwobj, btn, LWBTN_EVT_ONRELEASE);

                /* Check time validity for click event */
                if ((mstime - btn->time_change) >= LWBTN_TIME_CLICK_GET_PRESSED_MIN(btn)
                    && (mstime - btn->time_change) <= LWBTN_TIME_CLICK_GET_PRESSED_MAX(btn)) {

                    /*
                     * Increase consecutive clicks if max not reached yet
                     * and if time between two clicks is not too long
                     * 
                     * Otherwise we consider click as fresh one
                     */
                    if (btn->click.cnt > 0 && btn->click.cnt < LWBTN_CLICK_MAX_CONSECUTIVE(btn)
                        && (mstime - btn->click.last_time) < LWBTN_TIME_CLICK_MAX_MULTI(btn)) {
                        ++btn->click.cnt;
                    } else {
                        btn->click.cnt = 1;
                    }
                    btn->click.last_time = mstime;
                } else {
                    /*
                     * There was an on-release event, but timing
                     * for click event detection is outside allowed window.
                     * 
                     * If user has some consecutive clicks from previous clicks,
                     * these will be sent after the timeout window (and after the on-release event)
                     */
                }

#if LWBTN_CFG_CLICK_MAX_CONSECUTIVE_SEND_IMMEDIATELY
                /* 
                 * Depending on the configuration,
                 * this part will send on-click event immediately after release event,
                 * if maximum number of consecutive clicks has been reached.
                 */
                if (btn->click.cnt > 0 && btn->click.cnt == LWBTN_CLICK_MAX_CONSECUTIVE(btn)) {
                    lwobj->evt_fn(lwobj, btn, LWBTN_EVT_ONCLICK);
                    btn->click.cnt = 0;
                }
#endif /* LWBTN_CFG_CLICK_MAX_CONSECUTIVE_SEND_IMMEDIATELY */
            }
        }

        /*
         * Button just pressed
         */
        else {
            /* Do nothing - things are handled after debounce period */
        }
        btn->time_change = mstime;
#if LWBTN_CFG_USE_KEEPALIVE
        btn->keepalive.last_time = mstime;
        btn->keepalive.cnt = 0;
#endif /* LWBTN_CFG_USE_KEEPALIVE */
    }

    /*
     * Button is still pressed
     */
    else if (new_state) {
        /* 
         * Handle debounce and send on-press event
         *
         * This is when we detect valid press
         */
        if (!(btn->flags & LWBTN_FLAG_ONPRESS_SENT)) {
            /* Check minimum stable time */
            if ((mstime - btn->time_change) >= LWBTN_TIME_DEBOUNCE_GET_MIN(btn)) {
#if !LWBTN_CFG_CLICK_MAX_CONSECUTIVE_SEND_IMMEDIATELY
                /*
                 * Depending on the configuration,
                 * this part will send on-click event just before the next on-press release,
                 * if maximum number of consecutive clicks has been reached.
                 */
                if (btn->click.cnt > 0 && btn->click.cnt == LWBTN_CLICK_MAX_CONSECUTIVE(btn)) {
                    lwobj->evt_fn(lwobj, btn, LWBTN_EVT_ONCLICK);
                    btn->click.cnt = 0;
                }
#endif /* !LWBTN_CFG_CLICK_MAX_CONSECUTIVE_SEND_IMMEDIATELY */

                /* Start with new on-press */
                btn->flags |= LWBTN_FLAG_ONPRESS_SENT;
                lwobj->evt_fn(lwobj, btn, LWBTN_EVT_ONPRESS);

#if LWBTN_CFG_USE_KEEPALIVE
                /* Set keep alive time */
                btn->keepalive.last_time = mstime;
#endif /* LWBTN_CFG_USE_KEEPALIVE */
            }
        }

        /*
         * Handle keep alive, but only if on-press event has been sent
         *
         * Keep alive is sent when valid press is being detected
         */
        else {
#if LWBTN_CFG_USE_KEEPALIVE
            if ((mstime - btn->keepalive.last_time) >= LWBTN_TIME_KEEPALIVE_PERIOD(btn)) {
                btn->keepalive.last_time += LWBTN_TIME_KEEPALIVE_PERIOD(btn);
                ++btn->keepalive.cnt;
                lwobj->evt_fn(lwobj, btn, LWBTN_EVT_KEEPALIVE);
            }
#endif /* LWBTN_CFG_USE_KEEPALIVE */
        }
    }

    /*
     * Button is still released
     */
    else {
        /* 
         * Based on te configuration, this part of the code
         * will send on-click event after certain timeout.
         * 
         * This feature is useful if users prefers multi-click feature
         * that is reported only after last click event happened,
         * including number of clicks made by user
         */
        if (btn->click.cnt > 0) {
            if ((mstime - btn->click.last_time) >= LWBTN_TIME_CLICK_MAX_MULTI(btn)) {
                lwobj->evt_fn(lwobj, btn, LWBTN_EVT_ONCLICK);
                btn->click.cnt = 0;
            }
        }
    }
    btn->old_state = new_state;
}

/**
 * \brief           Initialize button manager
 * \param[in]       lwobj: LwBTN instance. Set to `NULL` to use default one
 * \param[in]       btns: Array of buttons to process
 * \param[in]       btns_cnt: Number of buttons to process
 * \param[in]       get_state_fn: Pointer to function providing button state on demand.
 *                      May be set to `NULL` when \ref LWBTN_CFG_GET_STATE_MODE is set to manual.
 * \param[in]       evt_fn: Button event function callback
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwbtn_init_ex(lwbtn_t* lwobj, lwbtn_btn_t* btns, uint16_t btns_cnt, lwbtn_get_state_fn get_state_fn,
              lwbtn_evt_fn evt_fn) {
    lwobj = LWBTN_GET_LWOBJ(lwobj);

    if (btns == NULL || btns_cnt == 0 || evt_fn == NULL
#if LWBTN_CFG_GET_STATE_MODE == LWBTN_GET_STATE_MODE_CALLBACK
        || get_state_fn == NULL /* Parameter is a must only in callback-only mode */
#endif                          /* LWBTN_CFG_GET_STATE_MODE == LWBTN_GET_STATE_MODE_CALLBACK */
    ) {
        return 0;
    }

    memset(lwobj, 0x00, sizeof(*lwobj));
    lwobj->btns = btns;
    lwobj->btns_cnt = btns_cnt;
    lwobj->evt_fn = evt_fn;
#if LWBTN_CFG_GET_STATE_MODE != LWBTN_GET_STATE_MODE_MANUAL
    lwobj->get_state_fn = get_state_fn;
#else
    (void)get_state_fn; /* May be unused */
#endif /* LWBTN_CFG_GET_STATE_MODE != LWBTN_GET_STATE_MODE_MANUAL */
    return 1;
}

/**
 * \brief           Button processing function, that reads the inputs and makes actions accordingly.
 * 
 * It checks state of all the buttons, linked to the specific LwBTN instance (group).
 * 
 * \param[in]       lwobj: LwBTN instance. Set to `NULL` to use default one
 * \param[in]       mstime: Current time in milliseconds
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwbtn_process_ex(lwbtn_t* lwobj, uint32_t mstime) {
    lwobj = LWBTN_GET_LWOBJ(lwobj);

    /* Process all buttons */
    for (size_t index = 0; index < lwobj->btns_cnt; ++index) {
        prv_process_btn(lwobj, &lwobj->btns[index], mstime);
    }
    return 1;
}

/**
 * \brief           Process single button instance from the specific LwOBJ instance (group).
 * 
 * This feature can be used if application wants to process the button events only
 * when interrupt hits (as a trigger). It gives user higher autonomy to decide which 
 * and when it will call specific button processing.
 * 
 * \param[in]       lwobj: LwBTN instance. Set to `NULL` to use default one
 * \param[in]       btn: Button object. Must not be set to `NULL`.
 * \param[in]       mstime: Current time in milliseconds
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwbtn_process_btn_ex(lwbtn_t* lwobj, lwbtn_btn_t* btn, uint32_t mstime) {
    if (btn != NULL) {
        prv_process_btn(LWBTN_GET_LWOBJ(lwobj), btn, mstime);
        return 1;
    }
    return 0;
}

/**
 * \brief           Set button state to either "active" or "inactive".
 * \param[in]       btn: Button instance
 * \param[in]       state: New button state. `1` is for active (pressed), `0` is for inactive (released).
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwbtn_set_btn_state(lwbtn_btn_t* btn, uint8_t state) {
#if LWBTN_CFG_GET_STATE_MODE != LWBTN_GET_STATE_MODE_CALLBACK
    btn->curr_state = state;
    btn->flags |= LWBTN_FLAG_MANUAL_STATE;
    return 1;
#else  /* LWBTN_CFG_GET_STATE_MODE != LWBTN_GET_STATE_MODE_CALLBACK */
    (void)btn;
    (void)state;
    return 0;
#endif /* LWBTN_CFG_GET_STATE_MODE != LWBTN_GET_STATE_MODE_CALLBACK */
}

/**
 * \brief           Check if button is active.
 * Active is considered when initial debounce period has been a pass.
 * This is the period between on-press and on-release events.
 * 
 * \param[in]       btn: Button handle to check
 * \return          `1` if active, `0` otherwise
 */
uint8_t
lwbtn_is_btn_active(const lwbtn_btn_t* btn) {
    return btn != NULL && (btn->flags & LWBTN_FLAG_ONPRESS_SENT);
}
