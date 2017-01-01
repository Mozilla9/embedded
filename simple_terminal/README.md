# README #

Simple terminal lib that allows to connect debug command interface to your board.


**How to use it**

```

/* Callback for the command 'ferase' */
static void clbk_ferase(const uint8_t * const param)
{
    /* example */
    earse_flash_mem();
}

/* command descriptor - name + callback */
static __sterminal_cmd cmd_ferase = {
        .cmd = (uint8_t *)"ferase",
        .callback = clbk_ferase
};

/* NULL terminated array of commands */
static __sterminal_cmd * commands[] = {&cmd_ferase, NULL};


/* buffer for terminal */
static uint8_t sterminal_buf[64];
static __sterminal_buffer stbuffer = {
        .buf = sterminal_buf,
        .size = 64
};


/* terminal descriptor - protocol descriptor + commands array */
static const __sterminal_descriptor descriptor = {
        .START_TOKEN = 0xff,     /* protocol without start prefix */
        .END_TOKEN = '\r',
        .commands = commands
};


/* management structure handle */
__sterminal_handle dbg_terminal;


/* initializing handle before using it */
void init_dbg_terminal(void)
{
    create_sterminal(&dbg_terminal, &descriptor, &stbuffer);
}
