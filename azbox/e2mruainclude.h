//Additional Azbox
enum key_command {
KEY_COMMAND_QUIT_ALL = 100,
KEY_COMMAND_QUIT,
KEY_COMMAND_PLAY,
KEY_COMMAND_PAUSE,
KEY_COMMAND_RESUME,
KEY_COMMAND_STOP,
KEY_COMMAND_SEEK_TO_TIME,
KEY_COMMAND_SEEK_TO_PERCENT,
KEY_COMMAND_NEXT_PICT,
KEY_COMMAND_FAST_FWD_ALL_FRAMES,
KEY_COMMAND_SLOW_FWD_ALL_FRAMES,
KEY_COMMAND_IFRAMES_FWD,
KEY_COMMAND_IFRAMES_BWD,
KEY_COMMAND_SILENT_FWD,
KEY_COMMAND_SILENT_BWD,
KEY_COMMAND_SWITCH_VIDEO,
KEY_COMMAND_SWITCH_AUDIO,
KEY_COMMAND_SWITCH_PROGRAM,
KEY_COMMAND_SWITCH_SUBS,
KEY_COMMAND_SWITCH_MULTICAST,
KEY_COMMAND_APPLY_AV_DELAY,
KEY_COMMAND_SUBS_CHANGE_DELAY,
KEY_COMMAND_SUBS_INCREASE_FONT_SIZE,
KEY_COMMAND_SUBS_DECREASE_FONT_SIZE,
KEY_COMMAND_SUBS_INCREASE_POS_Y,
KEY_COMMAND_SUBS_DECREASE_POS_Y,
KEY_COMMAND_SUBS_SWITCH_ENCODING,
KEY_COMMAND_SUBS_RESET_ALL,
KEY_COMMAND_SUBS_CHANGE_COLOR,
KEY_COMMAND_DEBUG,
KEY_COMMAND_PRINT_INFO,
KEY_COMMAND_FULL_SCREEN,
KEY_COMMAND_HALF_SCREEN,
KEY_COMMAND_INCREASE_SIZE,
KEY_COMMAND_DECREASE_SIZE,
KEY_COMMAND_MOVE_LEFT,
KEY_COMMAND_MOVE_RIGHT,
KEY_COMMAND_MOVE_TOP,
KEY_COMMAND_MOVE_BOTTOM,
KEY_COMMAND_NONLINEAR_WIDTH,
KEY_COMMAND_NONLINEAR_LEVEL,
KEY_COMMAND_SWITCH_SCALER,
KEY_COMMAND_HELP,
KEY_COMMAND_FAST_FWD_WITH_AUDIO,
KEY_COMMAND_SLOW_FWD_WITH_AUDIO,
KEY_COMMAND_PRINT_TXT,
SPECIAL_KEY_COMMAND_IFRAMES_FWD,
SPECIAL_KEY_COMMAND_IFRAMES_BWD,
SPECIAL_KEY_COMMAND_NEXT_AUDIO,
SPECIAL_KEY_COMMAND_NEXT_SUBS,
};

enum custom_command {
CUSTOM_COMMAND_GETLENGTH  = 200,
CUSTOM_COMMAND_GETPOSITION,
CUSTOM_COMMAND_AUDIOGETPOSITION,
CUSTOM_COMMAND_SEEK_RELATIVE_FWD,
CUSTOM_COMMAND_SEEK_RELATIVE_BWD,
CUSTOM_COMMAND_SUBS_COUNT,
CUSTOM_COMMAND_GET_SUB_BY_ID,
CUSTOM_COMMAND_AUDIO_COUNT,
CUSTOM_COMMAND_GET_AUDIO_BY_ID,
CUSTOM_COMMAND_AUDIO_CUR_STREAM,
CUSTOM_COMMAND_SUBS_CUR_STREAM,
CUSTOM_COMMAND_TRICK_SEEK,
CUSTOM_COMMAND_SET_SUB_SIZE,
CUSTOM_COMMAND_SET_SUB_ENCODING,
CUSTOM_COMMAND_SET_SUB_POS,
};

enum event_msg {
EVENT_MSG_FDOPEN = 300,
EVENT_MSG_PLAYBACK_STARTED,
EVENT_MSG_STOPPED,
EVENT_MSG_PAUSED,
EVENT_MSG_BUFFERING,
EVENT_MSG_EOS,
EVENT_MSG_SUB_CHANGED,
};

//int fd_cmd, fd_in, fd_out, fd_event, msg;
