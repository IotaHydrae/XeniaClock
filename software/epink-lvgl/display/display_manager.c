#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "epd.h"

static struct display_module *g_pt_disp_module = NULL;
static uint32_t g_module_id = 0;

/* called by each handler like statusbar,chart ... */
int register_module( struct display_module *module )
{
    struct display_module *p_tmp;
    
    if( !module ) {
        return -1;
    }
    
    printf( "register module: %s\n", module->name );
    
    if( !g_pt_disp_module ) {
        pr_debug( "first handler\n" );
        g_pt_disp_module = module;  /* first node */
    }
    else {
        p_tmp = g_pt_disp_module;
        while( p_tmp->p_next ) {
            p_tmp = p_tmp->p_next;
        }
        p_tmp->p_next = module;
    }
    module->id = g_module_id++;
    module->p_next = NULL;
    
    return 0;
}

struct display_module *request_disp_module(char *name)
{
    struct display_module *p_tmp = g_pt_disp_module;
    
    while (p_tmp) {
        if (0 == strcmp(p_tmp->name, name)) {
            return p_tmp;
        }
        p_tmp = p_tmp->p_next;
    }
}

void disp_modules_init( void )
{
    struct display_module *p_tmp = g_pt_disp_module;

    if( !p_tmp ) {
        pr_debug( "no handler\n" );
        return;
    }

    if( !p_tmp->p_next && p_tmp->ops.module_init ) {
        pr_debug( "only one handler, calling it's init func\n" );
        p_tmp->ops.module_init(DEFAULT_EPINK_UPDATE_MODE);
        return;
    }

    pr_debug( "more than one handler, calling all init func\n" );
    while( p_tmp ) {
        pr_debug( "init handler %s, id: %d\n", p_tmp->name, p_tmp->id );
        if( p_tmp->ops.module_init && p_tmp->ops.module_init(DEFAULT_EPINK_UPDATE_MODE) ) {
            pr_debug( "init handler %s failed\n", p_tmp->name );
            return;
        }
        p_tmp = p_tmp->p_next;
    }
}