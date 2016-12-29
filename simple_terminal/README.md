# README #

Simple terminal lib that allows to connect debug command interface to your board.

#### Example how use it


```

/**
 * Callback for the command 'ferase'.
 */
static void clbk_ferase(const uint8_t * const param)
{
    /* example */
    
    earse_flash_mem();
    printf("Erase end\r\n");
}


__sterminal_handle dbg_terminal;

static __sterminal_cmd cmd_ferase = {
        .cmd = (uint8_t *)"ferase",
        .callback = clbk_ferase
};

/* NULL terminated array */
static __sterminal_cmd * commands[] = {&cmd_ferase, NULL};

static uint8_t sterminal_buf[64];
static __sterminal_buffer stbuffer = {
        .buf = sterminal_buf,
        .size = 64
};

static const __sterminal_descriptor descriptor = {
        .START_TOKEN = 0xff,     /* protocol without start prefix */
        .END_TOKEN = '\r',
        .commands = commands
};


void init_dbg_terminal(void)
{
    create_sterminal(&dbg_terminal, &descriptor, &stbuffer);
}
