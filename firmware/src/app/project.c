/* SPDX-License-Identifier: GPL-3.0-only */
/* DUB-1 project: one flash object (OBJ_PROJ). A project of another format, or none, gives the
 * defaults. Part of the unity build, after app.h.
 *
 * The pieces (their recipes are the engine's, en_piece): each keeps its own settings as you leave it;
 * Tune and MIDI out are global. */
project_t proj;

const char *piece_name(int i) { return en_piece_name(i); }

static int global_par(int p) { return p == P_TUNE || p == P_MIDI; }

static void recipe(int i, slot_t *s)
{
    en_piece(i, s->par, &s->pool);
    s->pings_ok = 0;
    s->screen = 0;
}

/* DUB-1 1.0.0's project (format 8): the same, without the pings; read, and kept */
typedef struct {
    int16_t par[P_NPARAMS];
    uint16_t pool;
} slot8_t;
typedef struct {
    uint32_t magic, format;
    int16_t par[P_NPARAMS];
    uint16_t pool;
    uint8_t leds, piece, screen, rsv0;
    slot8_t slot[NPIECES];
    uint8_t rsv[16];
} project8_t;
_Static_assert(sizeof(project_t) <= 3840u, "the project must fit one storage sector's payload");

static void take(const slot_t *s)                /* a piece's settings into play, the globals kept */
{
    int p;
    for (p = 0; p < P_NPARAMS; p++)
        if (!global_par(p))
            proj.par[p] = s->par[p];
    proj.pool = s->pool;
    proj.screen = (uint8_t)(s->screen % 3u);      /* (each piece its own view) */
}

static void keep(void)                           /* the piece playing, into its slot (with its ping loops) */
{
    int p;
    for (p = 0; p < P_NPARAMS; p++)
        proj.slot[proj.piece].par[p] = proj.par[p];
    proj.slot[proj.piece].pool = proj.pool;
    proj.slot[proj.piece].screen = proj.screen;
    en_pings_get(proj.slot[proj.piece].pings);
    proj.slot[proj.piece].pings_ok = 1;
}

void project_defaults(void)
{
    int i;
    memset(&proj, 0, sizeof proj);
    proj.magic = PROJ_MAGIC;
    proj.format = PROJ_FORMAT;
    for (i = 0; i < NPIECES; i++)
        recipe(i, &proj.slot[i]);
    for (i = 0; i < P_NPARAMS; i++)
        proj.par[i] = en_param_info(i)->def;
    take(&proj.slot[0]);
    proj.leds = 1;
}

static int pool_ok(uint32_t p)                   /* the root and at least two more notes */
{
    int n = 0, i;
    for (i = 0; i < 12; i++)
        n += (int)(p >> i & 1u);
    return (p & 1u) && !(p >> 12) && n >= 3;
}

static int pars_ok(const int16_t *par)
{
    int i;
    for (i = 0; i < P_NPARAMS; i++) {
        const en_param_t *p = en_param_info(i);
        if (par[i] < p->lo || par[i] > p->hi)
            return 0;
    }
    return 1;
}

static int project_valid(void)                  /* an older format: the recipes again */
{
    int i;
    if (proj.magic != PROJ_MAGIC || !pool_ok(proj.pool) || !pars_ok(proj.par))
        return 0;
    if (proj.format != PROJ_FORMAT || proj.piece >= NPIECES)
        return 0;
    for (i = 0; i < NPIECES; i++)
        if (!pool_ok(proj.slot[i].pool) || !pars_ok(proj.slot[i].par))
            return 0;
    return 1;
}

static void from8(const project8_t *o)          /* 1.0.0's project: everything kept, the pings grown afresh */
{
    int i;
    memset(&proj, 0, sizeof proj);
    proj.magic = o->magic;
    proj.format = PROJ_FORMAT;
    memcpy(proj.par, o->par, sizeof proj.par);
    proj.pool = o->pool;
    proj.leds = o->leds, proj.piece = o->piece, proj.screen = o->screen;
    for (i = 0; i < NPIECES; i++) {
        memcpy(proj.slot[i].par, o->slot[i].par, sizeof proj.slot[i].par);
        proj.slot[i].pool = o->slot[i].pool;
    }
}

static void gaps9(int16_t *par)                  /* before 10: an empty slot is a gap (its root, left as it was, is not
                                                  * a degree of the walk's) */
{
    int i;
    for (i = 0; i < EN_NPROG; i++)
        if (!par[P_CQ0 + i])
            par[P_CR0 + i] = 0, par[P_CX0 + i] = 0;
    par[P_PMODE] = par[P_PMODE] ? 3 : 1;         /* Prog: Loop; Random: the walk, now Wander */
}

int project_load(void)
{
    static project_t buf;
    int n, old = 0, i;
    memset(&buf, 0, sizeof buf);
    n = plat_store_load(OBJ_PROJ, &buf, sizeof buf);
    if (n >= 8 && buf.magic == PROJ_MAGIC && buf.format == 8u && n >= (int)sizeof(project8_t)) {
        from8((const project8_t *)(const void *)&buf);
        old = 1;
    } else {
        proj = buf;
        if (proj.magic == PROJ_MAGIC && proj.format == 9u)
            proj.format = PROJ_FORMAT, old = 1;  /* (the same layout) */
    }
    if (old) {
        gaps9(proj.par);
        for (i = 0; i < NPIECES; i++)
            gaps9(proj.slot[i].par);
    }
    if (n >= 8 && project_valid())
        return 0;
    project_defaults();
    return -1;
}

int project_save(void)
{
    keep();
    return plat_store_save(OBJ_PROJ, &proj, sizeof proj);
}

void project_piece(int i)
{
    keep();
    proj.piece = (uint8_t)((i % NPIECES + NPIECES) % NPIECES);
    take(&proj.slot[proj.piece]);
}

void project_piece_reset(void)
{
    slot_t s;
    recipe(proj.piece, &s);
    s.screen = proj.screen;                      /* (the view stays as it is) */
    take(&s);
    proj.slot[proj.piece].pings_ok = 0;          /* (its loops grown afresh too) */
}

void project_apply(void)                         /* (then en_regrow keeps the piece's loops, if it has them) */
{
    int i;
    const slot_t *s = &proj.slot[proj.piece];
    for (i = 0; i < P_NPARAMS; i++)
        en_set(i, proj.par[i]);
    en_pool(proj.pool);
    en_pings(s->pings_ok ? s->pings : 0);
}
