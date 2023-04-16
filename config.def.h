#include <X11/XF86keysym.h>
/* appearance */
static const int sloppyfocus               = 1;  /* focus follows mouse */
static const int mousefollowsfocus         = 0;  /* mouse follows focus */
static const int bypass_surface_visibility = 0;  /* 1 means idle inhibitors will disable idle tracking even if it's surface isn't visible  */
static const int smartgaps                 = 0;  /* 1 means no outer gap when there is only one window */
static const int monoclegaps               = 0;  /* 1 means outer gaps in monocle layout */
static const unsigned int borderpx         = 1;  /* border pixel of windows */
static const unsigned int gappih           = 10; /* horiz inner gap between windows */
static const unsigned int gappiv           = 10; /* vert inner gap between windows */
static const unsigned int gappoh           = 10; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov           = 10; /* vert outer gap between windows and screen edge */
/*static const float bordercolor[]    = {0.5, 0.5, 0.5, 1.0};
static const float focuscolor[]     = {1.0, 0.0, 0.0, 1.0};*/
static const float bordercolor[]    = {0.157,0.173,0.204, 1.0};
static const float focuscolor[]     = {0.318,0.686,0.937, 1.0};
static const char *cursortheme            = NULL; /* theme from /usr/share/cursors/xorg-x11 */
static const unsigned int cursorsize       = 24;
/* To conform the xdg-protocol, set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]         = {0.1, 0.1, 0.1, 1.0};


/* pointer constraints */
static const int allow_constrain      = 1;

/* Autostart */
static const char *const autostart[] = {
        "wpaperd", NULL,
        NULL /* terminate */
};

/* tagging - tagcount must be no greater than 31 */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const int tagcount = 9;

static const Rule rules[] = {
	/* app_id     title       tags mask     isfloating  isterm  noswallow  monitor */
	/* examples:
	{ "Gimp",     NULL,       0,            1,          0,      1,         -1 },
	*/
	{ "firefox",  NULL,       0,       0,          0,      1,         -1 },
};

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* monitors */
static const MonitorRule monrules[] = {
	/* name       mfact nmaster scale layout       rotate/reflect                x    y */
	/* example of a HiDPI laptop monitor:
	{ "eDP-1",    0.5,  1,      2,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
	*/
    { "HDMI-A-1", 0.55, 2,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_270,       0,   690},
    { "DP-1",     0.55, 1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,    1081,840},
    { "DP-2",     0.5,  1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,    3641,990},
	/* defaults */
	{ NULL,       0.55, 1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
};

/* keyboard */
static const struct xkb_rule_names xkb_rules = {
	/* can specify fields: rules, model, layout, variant, options */
	/* example:
	.options = "ctrl:nocaps",
	*/
	.options = NULL,
};

static const int repeat_rate = 25;
static const int repeat_delay = 600;

/* Trackpad */
static const int tap_to_click = 1;
static const int tap_and_drag = 1;
static const int drag_lock = 1;
static const int natural_scrolling = 0;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;
/* You can choose between:
LIBINPUT_CONFIG_SCROLL_NO_SCROLL
LIBINPUT_CONFIG_SCROLL_2FG
LIBINPUT_CONFIG_SCROLL_EDGE
LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN
*/
static const enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;

/* You can choose between:
LIBINPUT_CONFIG_CLICK_METHOD_NONE
LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS
LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER
*/
static const enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;

/* You can choose between:
LIBINPUT_CONFIG_SEND_EVENTS_ENABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE
*/
static const uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;

/* You can choose between:
LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT
LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE
*/
static const enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
static const double accel_speed = 0.0;
/* You can choose between:
LIBINPUT_CONFIG_TAP_MAP_LRM -- 1/2/3 finger tap maps to left/right/middle
LIBINPUT_CONFIG_TAP_MAP_LMR -- 1/2/3 finger tap maps to left/middle/right
*/
static const enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

/* If you want to use the windows key for MODKEY, use WLR_MODIFIER_LOGO */
#define MODKEY WLR_MODIFIER_LOGO
#define MOD_ALT WLR_MODIFIER_ALT
#define MOD_CONTROL WLR_MODIFIER_CTRL
#define MOD_SHIFT WLR_MODIFIER_SHIFT
#define MOD_LOGO WLR_MODIFIER_LOGO
#define MOD_NONE 0

/* Prefix key */
#define PREFIXKEY XKB_KEY_t

#define TAGKEYS(KEY,SKEY,TAG) \
	{ 1, {{MODKEY,  KEY}},                      view,       {.ui = 1 << TAG} }, \
	{ 1, {{MODKEY|MOD_CONTROL, KEY}},           toggleview, {.ui = 1 << TAG} }, \
	{ 1, {{MODKEY|MOD_SHIFT, SKEY}},            tag,        {.ui = 1 << TAG} }, \
	{ 1, {{MODKEY|MOD_CONTROL|MOD_SHIFT, KEY}}, toggletag,  {.ui = 1 << TAG} } 

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[] = { "alacritty", NULL };
static const char *menucmd[] = { "bemenu-run", NULL };
// for cahnging the volume via alsa amixer //
static const char *upvol[] = { "pamixer", "-i", "3","&&" ,"volume-popup-dwl", NULL };
static const char *downvol[] = { "pamixer", "-d", "3","&&" ,"volume-popup-dwl", NULL };
// for muting/unmuting //
static const char *mute[] = { "pamixer", "-t;", "volume-popup-dwl", NULL };
// for spawning some programs //
static const char *btop[] = { "alacritty", "-e", "btop", NULL };
static const char *fileManager[] = { "nautilus", NULL };
static const char *termFileManager[] = { "alacritty", "e", "nnn", "-a", "-c", NULL };
static const char *screenshotClipboard[] = { "spectacle", "--region", "--background", "--copy-image", NULL };
static const char *screenshotSave[] = { "spectacle", "--region", "--background", NULL };
static const char *emacsclient[] = { "emacsclient", "-c" , "-a","\'emacs\'", NULL };

#include "shiftview.c"

static const Keychord keychords[] = {
	/* Note that Shift changes certain key codes: c -> C, 2 -> at, etc. */
	/* count key_sequences                           function          argument */
	{ 1, {{MODKEY, XKB_KEY_d}},                      spawn,            { .v = menucmd } },
	{ 1, {{MODKEY, XKB_KEY_Return}},                 spawn,            { .v = termcmd } },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_Return}},       spawn,            { .v = emacsclient } },
	{ 1, {{MODKEY, XKB_KEY_r}},                      spawn,            { .v = fileManager } },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_R}},            spawn,            { .v = termFileManager } },
	{ 1, {{MOD_ALT|MOD_CONTROL, XKB_KEY_Delete}},    spawn,            { .v = btop } },
	{ 1, {{MODKEY, XKB_KEY_s}},                      spawn,            { .v = screenshotSave } },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_s}},            spawn,            { .v = screenshotClipboard } },
	{ 1, {{MODKEY,XKB_KEY_b}},                       toggle_visibility, {0}},// for bar
	{ 1, {{MODKEY, XKB_KEY_n}},                      focusstack,       {.i = +1} },
	{ 1, {{MODKEY, XKB_KEY_e}},                      focusstack,       {.i = -1} },
	{ 1, {{MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_N}},   movestack,      {.i = 1} },
	{ 1, {{MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_E}},   movestack,      {.i = -1} },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_I}},            incnmaster,       {.i = +1} },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_M}},            incnmaster,       {.i = -1} },
	{ 1, {{MODKEY, XKB_KEY_m}},                      setmfact,         {.f = -0.05} },
	{ 1, {{MODKEY, XKB_KEY_i}},                      setmfact,         {.f = +0.05} },
	{ 2, {{MODKEY, XKB_KEY_g}, {MOD_NONE,XKB_KEY_e}},incgaps,       {.i = 1 } },
	{ 2, {{MODKEY, XKB_KEY_g}, {MOD_NONE,XKB_KEY_n}},incgaps,       {.i = -1 } },
	{ 2, {{MODKEY, XKB_KEY_g}, {MOD_SHIFT,XKB_KEY_I}},incogaps,      {.i = 1 } },
	{ 2, {{MODKEY, XKB_KEY_g}, {MOD_SHIFT,XKB_KEY_M}},incogaps,      {.i = -1 } },
	{ 2, {{MODKEY, XKB_KEY_g}, {MOD_SHIFT,XKB_KEY_E}},incigaps,      {.i = 1 } },
	{ 2, {{MODKEY, XKB_KEY_g}, {MOD_SHIFT,XKB_KEY_N}},incigaps,      {.i = -1 } },
	{ 2, {{MODKEY, XKB_KEY_g}, {MOD_NONE,XKB_KEY_g}},togglegaps,     {0} },
	{ 2, {{MODKEY, XKB_KEY_g}, {MOD_SHIFT,XKB_KEY_G}},defaultgaps,    {0} },
	{ 3, {{MODKEY, XKB_KEY_g}, {MOD_CONTROL, XKB_KEY_i}, {MOD_NONE,XKB_KEY_i}},incihgaps,     {.i = 1 } },
	{ 3, {{MODKEY, XKB_KEY_g}, {MOD_CONTROL, XKB_KEY_i}, {MOD_NONE,XKB_KEY_m}},incihgaps,     {.i = -1 } },
	{ 3, {{MODKEY, XKB_KEY_g}, {MOD_CONTROL, XKB_KEY_i}, {MOD_NONE,XKB_KEY_e}},incivgaps,     {.i = 1 } },
	{ 3, {{MODKEY, XKB_KEY_g}, {MOD_CONTROL, XKB_KEY_i}, {MOD_NONE,XKB_KEY_n}},incivgaps,     {.i = -1 } },
	{ 3, {{MODKEY, XKB_KEY_g}, {MOD_CONTROL, XKB_KEY_o}, {MOD_NONE,XKB_KEY_i}},incohgaps,     {.i = 1 } },
	{ 3, {{MODKEY, XKB_KEY_g}, {MOD_CONTROL, XKB_KEY_o}, {MOD_NONE,XKB_KEY_m}},incohgaps,     {.i = -1 } },
	{ 3, {{MODKEY, XKB_KEY_g}, {MOD_CONTROL, XKB_KEY_o}, {MOD_NONE,XKB_KEY_e}},incovgaps,     {.i = 1 } },
	{ 3, {{MODKEY, XKB_KEY_g}, {MOD_CONTROL, XKB_KEY_o}, {MOD_NONE,XKB_KEY_n}},incovgaps,     {.i = -1 } },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_Return}},       zoom,             {0} },
	{ 1, {{MODKEY, XKB_KEY_Tab}},                    view,             {0} },
	{ 1, {{MODKEY, XKB_KEY_u}},                      shiftview,      { .i = -1 } },
	{ 1, {{MODKEY, XKB_KEY_l}},                      shiftview,      { .i = 1 } },
	{ 1, {{MODKEY, XKB_KEY_q}},                      killclient,       {0} },
	{ 1, {{MODKEY, XKB_KEY_t}},                      setlayout,        {.v = &layouts[0]} },
	{ 1, {{MODKEY, XKB_KEY_f}},                      setlayout,        {.v = &layouts[1]} },
	{ 1, {{MODKEY, XKB_KEY_j}},                      setlayout,        {.v = &layouts[2]} },
	{ 1, {{MODKEY, XKB_KEY_space}},                  setlayout,        {0} },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_space}},        togglefloating,   {0} },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_f}},            togglefullscreen, {0} },
	{ 1, {{MODKEY, XKB_KEY_0}},                      view,             {.ui = ~0} },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_parenright}},   tag,              {.ui = ~0} },
	{ 1, {{MODKEY, XKB_KEY_comma}},                  focusmon,         {.i = WLR_DIRECTION_LEFT} },
	{ 1, {{MODKEY, XKB_KEY_period}},                 focusmon,         {.i = WLR_DIRECTION_RIGHT} },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_less}},         tagmon,           {.i = WLR_DIRECTION_LEFT} },
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_greater}},      tagmon,           {.i = WLR_DIRECTION_RIGHT} },
	TAGKEYS(          XKB_KEY_1, XKB_KEY_exclam,                     0),
	TAGKEYS(          XKB_KEY_2, XKB_KEY_at,                         1),
	TAGKEYS(          XKB_KEY_3, XKB_KEY_numbersign,                 2),
	TAGKEYS(          XKB_KEY_4, XKB_KEY_dollar,                     3),
	TAGKEYS(          XKB_KEY_5, XKB_KEY_percent,                    4),
	TAGKEYS(          XKB_KEY_6, XKB_KEY_asciicircum,                5),
	TAGKEYS(          XKB_KEY_7, XKB_KEY_ampersand,                  6),
	TAGKEYS(          XKB_KEY_8, XKB_KEY_asterisk,                   7),
	TAGKEYS(          XKB_KEY_9, XKB_KEY_parenleft,                  8),
	{ 1, {{MODKEY|MOD_SHIFT, XKB_KEY_Q}},            quit,           {0} },

	/* Ctrl-Alt-Backspace and Ctrl-Alt-Fx used to be handled by X server */
	{ 1, {{WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_Terminate_Server}}, quit, {0} },
#define CHVT(n) { 1, {{WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_XF86Switch_VT_##n}}, chvt, {.ui = (n)} }
	CHVT(1), CHVT(2), CHVT(3), CHVT(4), CHVT(5), CHVT(6),
	CHVT(7), CHVT(8), CHVT(9), CHVT(10), CHVT(11), CHVT(12),
	{ 1, {{0,XF86XK_AudioRaiseVolume}}, spawn,{.v = upvol } },
	{ 1, {{0,XF86XK_AudioLowerVolume}}, spawn,{.v = downvol } },
	{ 1, {{0,XF86XK_AudioMute}},spawn,{.v = mute } },
};

static const Button buttons[] = {
	{ MODKEY, BTN_LEFT,   moveresize,     {.ui = CurMove} },
	{ MODKEY, BTN_MIDDLE, togglefloating, {0} },
	{ MODKEY, BTN_RIGHT,  moveresize,     {.ui = CurResize} },
};
