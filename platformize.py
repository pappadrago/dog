#!/usr/bin/env python3
"""
platformize.py - rende gli schemi 1..12 piu' "platform" (default; --schemi 1-12 | 3,5,7).

Per ogni schema (difficolta' crescente, vedi PLAN) aggiunge 3 sfide e sposta li' sopra i bonus (powerup/energia;
chiavi, interruttori e porte NON vengono toccati):

  A) SCALINATA  piattaforme sospese (salita di 2 tile, vuoto di 2 tile): salti precisi
  B) TRAMPOLINO una lastra a 6 tile da terra, raggiungibile solo col trampolino
  C) CASSA      un pilastro alto 4 tile: serve spingere la cassa fino al muro,
                salirci sopra e saltare (con B premuto per tutto il salto)

Cosa modifica, per ogni schema N:
  include/megadog_sN.h   (se esiste) mappa di collisione + casse[] + trampolini[] + posizioni item
  tiled/megadog_sN.tmx   layer "Livello tile 1" (tile 250/277) e "Collision" (437)

Uso (dalla radice del repo):
  python3 platformize.py --dry     # mostra solo cosa farebbe
  python3 platformize.py           # applica (crea .bak accanto ai file)
Rilancialo SOLO sui file originali (non e' idempotente): ripristina dai .bak.
Dopo: rigenera gli asset dal tmx (la tua pipeline Tiled -> bmp/json).
"""
import re, sys, shutil, pathlib

COLS, ROWS = 192, 32
TILE_TOP, TILE_BODY, COLL_TMX = 250, 277, 437
# n = numero di gradini, w = larghezza dei gradini (in tile)
PLAN = {1: dict(n=3, w=3), 2: dict(n=3, w=2), 3: dict(n=4, w=2), 4: dict(n=4, w=1),
        5: dict(n=3, w=2),   # vento: piu' clemente
        6: dict(n=4, w=1),
        7: dict(n=4, w=2),   # vento
        8: dict(n=5, w=1), 9: dict(n=4, w=2), 10: dict(n=5, w=1), 11: dict(n=5, w=2), 12: dict(n=5, w=1)}
MAX_CASSE, MAX_TRAMPOLINI = 8, 8
# arene dei boss (colonna del boss in spawn_boss_schema): niente strutture nei dintorni
BOSS_COL = {4: 172, 8: 165, 12: 166, 16: 165}
DRY = '--dry' in sys.argv


def parse_schemi():
    """--schemi 1-12 | 3,5,7  (default: tutti quelli di PLAN)"""
    for i, a in enumerate(sys.argv):
        if a == '--schemi' and i + 1 < len(sys.argv):
            out = set()
            for part in sys.argv[i + 1].split(','):
                if '-' in part:
                    a0, b0 = part.split('-'); out.update(range(int(a0), int(b0) + 1))
                else:
                    out.add(int(part))
            return sorted(x for x in out if x in PLAN)
    return sorted(PLAN)
ROOT = pathlib.Path('.')

MAP_RE = re.compile(r'(collision_map\[collision_map_columns \* collision_map_rows\] =\s*\{\n)(.*?)(\n\s*\};)', re.S)


def read_map(txt):
    body = MAP_RE.search(txt).group(2)
    rows = [[int(v) for v in l.strip().rstrip(',').split(',')] for l in body.strip().split('\n') if l.strip()]
    assert len(rows) == ROWS and all(len(r) == COLS for r in rows), 'mappa .h di dimensioni inattese'
    return rows


def write_map(txt, rows):
    body = '\n'.join(','.join(map(str, r)) + ',' for r in rows)
    return MAP_RE.sub(lambda m: m.group(1) + body + m.group(3), txt, count=1)


def tmx_grid(txt, name):
    m = re.search(r'(<layer id="\d+" name="%s"[^>]*>\s*<data encoding="csv">\n)(.*?)(\n</data>)' % re.escape(name), txt, re.S)
    rows = [[int(v) for v in l.rstrip(',').split(',')] for l in m.group(2).split('\n')]
    assert len(rows) == ROWS and all(len(r) == COLS for r in rows), 'layer tmx "%s" inatteso' % name
    return rows


def tmx_put(txt, name, rows):
    body = ',\n'.join(','.join(map(str, r)) for r in rows)
    pat = re.compile(r'(<layer id="\d+" name="%s"[^>]*>\s*<data encoding="csv">\n)(.*?)(\n</data>)' % re.escape(name), re.S)
    return pat.sub(lambda m: m.group(1) + body + m.group(3), txt, count=1)


def array_block(txt, name):
    m = re.search(r'constexpr \w+ %s\[\] = \{\n(.*?)\n    \};' % name, txt, re.S)
    return m.group(1) if m else ''


def first_nums(line):
    m = re.match(r'\s*\{\s*(-?\d+)(?:\s*,\s*(-?\d+))?(?:\s*,\s*(-?\d+))?(?:\s*,\s*(-?\d+))?', line)
    return [int(g) for g in m.groups() if g is not None] if m else []


def blocked_columns(txt, schema=0):
    """Colonne da non toccare: porte, scale, spuntoni, lava, piattaforme, nemici, ecc."""
    cols = set()

    def add(a, b, pad):
        cols.update(range(a - pad, b + pad + 1))

    for line in array_block(txt, 'doors').split('\n'):
        n = first_nums(line)
        if n: add(n[0] // 16, n[0] // 16, 4)
    for line in array_block(txt, 'scale').split('\n'):
        n = first_nums(line)
        if n: add(n[0], n[0], 3)
    for line in array_block(txt, 'spikes').split('\n'):
        n = first_nums(line)
        if n: add(n[0], n[0], 2)
    for name in ('lava', 'piattaforme'):
        for line in array_block(txt, name).split('\n'):
            n = first_nums(line)
            if len(n) >= 3: add(n[0], n[2], 3)
    for name in ('piattaforme_mobili', 'oggetti_percorso', 'snakes'):
        for line in array_block(txt, name).split('\n'):
            n = first_nums(line)
            if len(n) >= 4: add((n[0] - n[2]) // 16, (n[0] + n[2]) // 16, 3)
    for name in ('enemies', 'casse', 'trampolini'):
        for line in array_block(txt, name).split('\n'):
            m = re.search(r'\{\s*[\w:]+,\s*[\w:| ]+,\s*(\d+),\s*(\d+)', line) if name == 'enemies' \
                else re.search(r'\{\s*(?:\w+,\s*)?(\d+)\s*,\s*(\d+)', line)
            if m: add(int(m.group(1)) // 16, int(m.group(1)) // 16, 2)
    if schema in BOSS_COL:
        add(BOSS_COL[schema], BOSS_COL[schema], 12)
    return cols


def ground_row(grid, x):
    for r in range(ROWS):
        if grid[r][x] == 1: return r
    return None


def find_site(grid, lo, hi, up, target, blocked, used):
    best = None
    for x0 in range(1 - lo, COLS - hi):
        cols = range(x0 + lo, x0 + hi)
        if any(c in blocked or c in used for c in cols): continue
        g = ground_row(grid, cols[0])
        if g is None or g - up < 2: continue
        if any(ground_row(grid, c) != g for c in cols): continue
        if any(grid[r][c] != 0 for c in cols for r in range(g - up, g)): continue
        s = abs(x0 - target)
        if best is None or s < best[0]: best = (s, x0, g)
    return best


def process(n):
    hp = ROOT / 'include' / ('megadog_s%d.h' % n)
    tp = ROOT / 'tiled' / ('megadog_s%d.tmx' % n)
    if not hp.exists() or not tp.exists():
        print('schema %d: file mancanti, saltato' % n); return
    txt, tmx = hp.read_text(), tp.read_text()
    if '// platformize' in txt or '// spostato:' in txt:
        print('schema %d: gia\' modificato (ripristina dai .bak per rifarlo), saltato' % n); return
    grid = read_map(txt)
    tile, coll = tmx_grid(tmx, 'Livello tile 1'), tmx_grid(tmx, 'Collision')
    blocked, used = blocked_columns(txt, n), set()
    cfg = PLAN[n]
    casse, tramp, rewards, log = [], [], [], []

    def block(c0, c1, r0, r1):  # tile incluse
        for r in range(r0, r1 + 1):
            for c in range(c0, c1 + 1):
                grid[r][c] = 1
                tile[r][c] = TILE_TOP if r == r0 else TILE_BODY
                coll[r][c] = COLL_TMX

    def reserve(x0, lo, hi):
        used.update(range(x0 + lo - 2, x0 + hi + 2))

    # --- A: scalinata di salti precisi ---
    w = cfg['w']
    s = None
    for nn in range(cfg['n'], 1, -1):   # se non c'e' spazio in altezza, meno gradini
        hiA = nn * (w + 2) + 3 + 1
        s = find_site(grid, -1, hiA, 2 * (nn + 1) + 1, int(COLS * .25), blocked, used)
        if s: break
    if s:
        _, x0, g = s
        for k in range(1, nn + 1):
            c = x0 + (k - 1) * (w + 2)
            block(c, c + w - 1, g - 2 * k, g - 2 * k)
        c = x0 + nn * (w + 2)
        top = g - 2 * (nn + 1)
        block(c, c + 2, top, top)
        rewards.append(('A scalinata', c + 1, top))
        reserve(x0, -1, hiA)
        log.append('A scalinata: x0=%d suolo=riga %d, %d gradini larghi %d, ledge in riga %d' % (x0, g, nn, w, top))
    # --- C: pilastro + cassa ---
    n_casse_esist = len([l for l in array_block(txt, 'casse').split('\n') if first_nums(l)])
    s = find_site(grid, -1, 10, 5, int(COLS * .5), blocked, used) if n_casse_esist < MAX_CASSE else None
    if s:
        _, x0, g = s
        block(x0 + 6, x0 + 8, g - 4, g - 1)
        casse.append((x0 + 1, g - 1))
        rewards.append(('C cassa', x0 + 7, g - 4))
        reserve(x0, -1, 10)
        log.append('C cassa: x0=%d suolo=riga %d, pilastro cols %d-%d (alto 4), cassa in col %d' % (x0, g, x0 + 6, x0 + 8, x0 + 1))
    # --- B: lastra + trampolino ---
    n_tramp_esist = len([l for l in array_block(txt, 'trampolini').split('\n') if first_nums(l)])
    s = find_site(grid, -1, 10, 8, int(COLS * .75), blocked, used) if n_tramp_esist < MAX_TRAMPOLINI else None
    if s:
        _, x0, g = s
        block(x0 + 4, x0 + 7, g - 6, g - 6)
        tramp.append((x0 + 2, g - 1))
        rewards.append(('B trampolino', x0 + 5, g - 6))
        reserve(x0, -1, 10)
        log.append('B trampolino: x0=%d suolo=riga %d, lastra cols %d-%d in riga %d, trampolino in col %d' % (x0, g, x0 + 4, x0 + 7, g - 6, x0 + 2))

    for nome, ok in (('A scalinata', any(r[0] == 'A scalinata' for r in rewards)),
                     ('C cassa', bool(casse)), ('B trampolino', bool(tramp))):
        if not ok: log.append('ATTENZIONE: nessun sito libero per la sfida %s (schema da fare a mano)' % nome)

    # --- def: casse / trampolini ---
    def add_entries(txt, arrname, typ, count_name, entries):
        if not entries: return txt
        body = ''.join('        %s,  // platformize\n' % e for e in entries)
        pat = re.compile(r'(constexpr %s %s\[\] = \{\n.*?)(\n    \};)' % (typ, arrname), re.S)
        if pat.search(txt):
            return pat.sub(lambda m: m.group(1) + '\n' + body.rstrip('\n') + m.group(2), txt, count=1)
        new = '    constexpr %s %s[] = {\n%s    };\n    constexpr int %s = sizeof(%s) / sizeof(%s[0]);\n' % (
            typ, arrname, body, count_name, arrname, arrname)
        old = '    // %s: nessuno\n' % arrname
        assert old in txt, 'impossibile inserire %s' % arrname
        return txt.replace(old, new, 1)

    if not rewards:
        print('schema %d: nessuna sfida piazzata, file non toccati' % n); [print('  ', l) for l in log]; return
    txt = add_entries(txt, 'casse', 'cassa_def', 'casse_count',
                      ['{ CASSA_SCATOLA, %d, %d, false }' % (c * 16 + 8, r * 16 + 8) for c, r in casse])
    txt = add_entries(txt, 'trampolini', 'trampolino_def', 'trampolini_count',
                      ['{ %d, %d }' % (c * 16 + 8, r * 16 + 8) for c, r in tramp])

    def fix_args(m):
        ind = re.match(r'\s*', m.group(2)).group(0)
        l1 = ind + 'casse, casse_count,\n' if re.search(r'cassa_def casse\[\]', txt) else m.group(2)
        l2 = ind + 'trampolini, trampolini_count,\n' if re.search(r'trampolino_def trampolini\[\]', txt) else m.group(3)
        return m.group(1) + l1 + l2
    txt = re.sub(r'(return \{\n)([^\n]*\n)([^\n]*\n)', fix_args, txt, count=1)

    # --- bonus: powerup prima, poi energie; mai chiavi/interruttori ---
    items = re.findall(r'^(\s*\{ ITEM_TIPO_(POWERUP|ENERGIA), [^\n]*)$', txt, re.M)
    order = [i for i in items if i[1] == 'POWERUP'] + [i for i in items if i[1] == 'ENERGIA']
    prio = {'B trampolino': 0, 'A scalinata': 1, 'C cassa': 2}   # i powerup nella sfida piu' dura
    for (full, _), (nome, c, r) in zip(order, sorted(rewards, key=lambda t: prio[t[0]])):
        new = re.sub(r'(\{ ITEM_TIPO_\w+, [^,]+, )\d+, \d+( \},)\s*(//.*)?$',
                     lambda m: '%s%d, %d%s  // spostato: %s' % (m.group(1), c * 16 + 8, r * 16 - 12, m.group(2), nome), full)
        txt = txt.replace(full, new, 1)
        log.append('   bonus -> %s' % nome)

    txt = write_map(txt, grid)
    tmx = tmx_put(tmx_put(tmx, 'Livello tile 1', tile), 'Collision', coll)
    if not DRY:
        for p in (hp, tp): shutil.copy(p, str(p) + '.bak')
        hp.write_text(txt); tp.write_text(tmx)
    print('schema %d:' % n); [print('  ', l) for l in log]


for n in parse_schemi():
    process(n)
print('(dry run: nessun file modificato)' if DRY else 'fatto: backup in *.bak')