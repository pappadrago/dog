#!/usr/bin/env python3
"""
platformize.py - rende gli schemi 1..16 piu' "platform" (default; --schemi 1-16 | 3,5,7).

Per ogni schema (difficolta' crescente, vedi PLAN) aggiunge sfide e sposta li' sopra i bonus
(powerup/energia; chiavi, interruttori e porte NON vengono toccati), oltre a un nemico di pattuglia:

  A) SCALINATA  piattaforme sospese (salita di 2 tile, vuoto di 2 tile): salti precisi
  B) TRAMPOLINO una lastra a 5 tile da terra, raggiungibile col trampolino
  C) CASSA      un pilastro alto 4 tile: serve spingere la cassa fino al muro,
                salirci sopra e saltare (con B premuto per tutto il salto)

Cosa modifica, per ogni schema N:
  include/megadog_sN.h   mappa di collisione + casse[] + trampolini[] + posizioni item + nemici[]
  tiled/megadog_sN.tmx   layer "Livello tile 1" (tile 250/277) e "Collision" (437)

Uso (dalla radice del repo):
  python3 platformize.py --dry     # mostra solo cosa farebbe
  python3 platformize.py           # applica (crea .bak accanto ai file, senza sovrascriverli)
Lo script e' idempotente: non ripete le piattaforme o i nemici gia' aggiunti.
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
        8: dict(n=5, w=1), 9: dict(n=4, w=2), 10: dict(n=5, w=1), 11: dict(n=5, w=2), 12: dict(n=5, w=1),
        13: dict(n=3, w=2), 14: dict(n=4, w=2), 15: dict(n=4, w=1), 16: dict(n=3, w=1)}
MAX_CASSE, MAX_TRAMPOLINI, MAX_ENEMIES = 8, 8, 24
# arene dei boss (colonna del boss in spawn_boss_schema): niente strutture nei dintorni
BOSS_COL = {4: 172, 8: 165, 12: 166, 16: 165}
DRY = '--dry' in sys.argv


def parse_schemi():
    """--schemi 1-16 | 3,5,7  (default: tutti quelli di PLAN)"""
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
        if n: add(n[0] // 16, n[0] // 16, 1)
    for line in array_block(txt, 'scale').split('\n'):
        n = first_nums(line)
        if n: add(n[0], n[0], 0)
    for line in array_block(txt, 'spikes').split('\n'):
        n = first_nums(line)
        if n: add(n[0], n[0], 0)
    for name in ('lava', 'piattaforme'):
        for line in array_block(txt, name).split('\n'):
            n = first_nums(line)
            if len(n) >= 3: add(n[0], n[2], 1)
    for name in ('piattaforme_mobili', 'oggetti_percorso', 'snakes'):
        for line in array_block(txt, name).split('\n'):
            n = first_nums(line)
            if len(n) >= 4: add((n[0] - n[2]) // 16, (n[0] + n[2]) // 16, 0)
    for name in ('enemies', 'casse', 'trampolini'):
        for line in array_block(txt, name).split('\n'):
            m = re.search(r'\{\s*[\w:]+,\s*[\w:| ]+,\s*(\d+),\s*(\d+)', line) if name == 'enemies' \
                else re.search(r'\{\s*(?:\w+,\s*)?(\d+)\s*,\s*(\d+)', line)
            if m: add(int(m.group(1)) // 16, int(m.group(1)) // 16, 0)
    for line in array_block(txt, 'items').split('\n'):
        m = re.search(r',\s*(-?\d+)\s*,\s*(-?\d+)\s*\}', line)
        if m: add(int(m.group(1)) // 16, int(m.group(1)) // 16, 0)
    if schema in BOSS_COL:
        add(BOSS_COL[schema], BOSS_COL[schema], 8)
    return cols


def find_site(grid, lo, hi, up, target, blocked, used):
    best = None
    for x0 in range(1 - lo, COLS - hi):
        cols = range(x0 + lo, x0 + hi)
        if any(c in blocked or c in used for c in cols): continue
        for g in range(max(up + 2, 2), ROWS):
            if any(grid[g][c] not in (1, 2) for c in cols): continue
            if any(grid[r][c] != 0 for c in cols for r in range(g - up, g)): continue
            # A parita' di distanza, preferisce il terreno principale ai piccoli davanzali.
            score = abs(x0 - target) + abs(g - 26) * 0.35
            if best is None or score < best[0]: best = (score, x0, g)
    return best


def entry_count(txt, name):
    return sum(1 for line in array_block(txt, name).split('\n') if re.match(r'\s*\{', line))


def validate_added_cells(original, updated, cells, support_sites):
    """Rifiuta sovrapposizioni, tetti bassi e strutture senza passaggio sottostante."""
    top_by_column = {}
    for c, r in cells:
        if original[r][c] != 0 or updated[r][c] != 1:
            raise ValueError('cella di piattaforma non libera prima della modifica: (%d,%d)' % (c, r))
        top_by_column[c] = min(r, top_by_column.get(c, r))
    for c, r in top_by_column.items():
        if r < 2 or updated[r - 1][c] != 0 or updated[r - 2][c] != 0:
            raise ValueError('spazio insufficiente sopra la piattaforma: (%d,%d)' % (c, r))
    for c0, c1, g in support_sites:
        if any(original[g][c] not in (1, 2) for c in range(c0, c1 + 1)):
            raise ValueError('appoggio discontinuo nelle colonne %d-%d' % (c0, c1))
        clear_run = 0
        max_clear_run = 0
        for c in range(c0, c1 + 1):
            clear_run = clear_run + 1 if updated[g - 1][c] == 0 else 0
            max_clear_run = max(max_clear_run, clear_run)
        if max_clear_run < 2:
            raise ValueError('nessuna via di fuga sotto la struttura (%d-%d)' % (c0, c1))


def add_enemy(txt, grid, blocked, used):
    if '// platformize: nemico aggiunto' in txt or entry_count(txt, 'enemies') >= MAX_ENEMIES:
        return txt, None
    site = find_site(grid, 0, 1, 2, int(COLS * .62), blocked, used)
    if not site:
        return txt, None
    _, x, g = site
    px, py = x * 16 + 8, g * 16 - 8
    entry = '        { enemy_tipo::MOVE_PATROL, enemy_attr::ATTR_NONE, %d, %d, 80 },  // platformize: nemico aggiunto' % (px, py)
    pat = re.compile(r'(constexpr enemy_def enemies\[\] = \{\n.*?)(\n    \};)', re.S)
    if not pat.search(txt):
        return txt, None
    txt = pat.sub(lambda m: m.group(1) + '\n' + entry + m.group(2), txt, count=1)
    return txt, (x, g)


def process(n):
    hp = ROOT / 'include' / ('megadog_s%d.h' % n)
    tp = ROOT / 'tiled' / ('megadog_s%d.tmx' % n)
    if not hp.exists() or not tp.exists():
        print('schema %d: file mancanti, saltato' % n); return
    txt, tmx = hp.read_text(), tp.read_text()
    geometry_done = '// platformize' in txt or '// spostato:' in txt
    grid = read_map(txt)
    tile, coll = tmx_grid(tmx, 'Livello tile 1'), tmx_grid(tmx, 'Collision')
    if any((v == 0 and coll[r][c] != 0) or (v == 1 and coll[r][c] != COLL_TMX) or
           (v == 2 and coll[r][c] != COLL_TMX + 1)
           for r, row in enumerate(grid) for c, v in enumerate(row)):
        raise ValueError('collision_map e layer Collision non corrispondono nello schema %d' % n)
    original_grid = [row[:] for row in grid]
    blocked, used = blocked_columns(txt, n), set()
    cfg = PLAN[n]
    casse, tramp, rewards, added_cells, support_sites, log = [], [], [], [], [], []

    def block(c0, c1, r0, r1):  # tile incluse
        for r in range(r0, r1 + 1):
            for c in range(c0, c1 + 1):
                if grid[r][c] != 0:
                    raise ValueError('la sfida sovrappone una collisione esistente: (%d,%d)' % (c, r))
                grid[r][c] = 1
                tile[r][c] = TILE_TOP if r == r0 else TILE_BODY
                coll[r][c] = COLL_TMX
                added_cells.append((c, r))

    def reserve(x0, lo, hi):
        used.update(range(x0 + lo - 2, x0 + hi + 2))

    if not geometry_done:
        # --- C: pilastro + cassa (priorita': evita che le altre sfide occupino il sito) ---
        n_casse_esist = entry_count(txt, 'casse')
        s = find_site(grid, 1, 9, 6, int(COLS * .5), blocked, used) if n_casse_esist < MAX_CASSE else None
        if s:
            _, x0, g = s
            support_sites.append((x0 + 1, x0 + 8, g))
            block(x0 + 6, x0 + 8, g - 4, g - 1)
            casse.append((x0 + 1, g - 1))
            rewards.append(('C cassa', x0 + 7, g - 4))
            reserve(x0, 1, 9)
            log.append('C cassa: x0=%d suolo=riga %d, pilastro cols %d-%d (alto 4), cassa in col %d' % (x0, g, x0 + 6, x0 + 8, x0 + 1))
        # --- B: lastra + trampolino ---
        n_tramp_esist = entry_count(txt, 'trampolini')
        s = find_site(grid, 2, 8, 7, int(COLS * .75), blocked, used) if n_tramp_esist < MAX_TRAMPOLINI else None
        if s:
            _, x0, g = s
            support_sites.append((x0 + 2, x0 + 7, g))
            block(x0 + 4, x0 + 7, g - 5, g - 5)
            tramp.append((x0 + 2, g - 1))
            rewards.append(('B trampolino', x0 + 5, g - 5))
            reserve(x0, 2, 8)
            log.append('B trampolino: x0=%d suolo=riga %d, lastra cols %d-%d in riga %d, trampolino in col %d' % (x0, g, x0 + 4, x0 + 7, g - 5, x0 + 2))
        # --- A: scalinata di salti precisi ---
        w = cfg['w']
        s = None
        for nn in range(cfg['n'], 0, -1):   # se non c'e' spazio, usa una salita breve ma precisa
            hiA = nn * (w + 2) + 3 + 1
            s = find_site(grid, 0, hiA, 2 * (nn + 1) + 2, int(COLS * .25), blocked, used)
            if s: break
        if s:
            _, x0, g = s
            support_sites.append((x0, x0 + hiA - 1, g))
            for k in range(1, nn + 1):
                c = x0 + (k - 1) * (w + 2)
                block(c, c + w - 1, g - 2 * k, g - 2 * k)
            c = x0 + nn * (w + 2)
            top = g - 2 * (nn + 1)
            block(c, c + 2, top, top)
            rewards.append(('A scalinata', c + 1, top))
            reserve(x0, 0, hiA)
            log.append('A scalinata: x0=%d suolo=riga %d, %d gradini larghi %d, ledge in riga %d' % (x0, g, nn, w, top))

    if not geometry_done:
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

    if not geometry_done and not rewards:
        log.append('nessuna sfida geometrica disponibile')
    if not geometry_done:
        validate_added_cells(original_grid, grid, added_cells, support_sites)
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

    if not geometry_done and rewards:
        txt = write_map(txt, grid)

    for _, c, r in rewards:
        blocked.update(range(c - 2, c + 3))
    enemy_already_added = '// platformize: nemico aggiunto' in txt
    txt, enemy_pos = add_enemy(txt, grid, blocked, used)
    if enemy_pos:
        log.append('nemico di pattuglia aggiunto in colonna %d, riga %d' % enemy_pos)
    elif enemy_already_added:
        log.append('nemico di pattuglia gia\' presente')
    elif entry_count(txt, 'enemies') >= MAX_ENEMIES:
        log.append('limite nemici raggiunto, aggiunta saltata')
    else:
        log.append('nessun punto sicuro per aggiungere un nemico')

    geometry_changed = not geometry_done and bool(rewards)
    enemy_changed = enemy_pos is not None
    if not geometry_changed and not enemy_changed:
        print('schema %d: nessuna modifica applicata' % n); [print('  ', l) for l in log]; return
    if geometry_changed:
        tmx = tmx_put(tmx_put(tmx, 'Livello tile 1', tile), 'Collision', coll)
    if not DRY:
        changed_paths = [hp, tp] if geometry_changed else [hp]
        backups = [pathlib.Path(str(p) + '.bak') for p in changed_paths]
        if any(p.exists() for p in backups):
            raise FileExistsError('backup gia\' esistente; non sovrascritto: %s' %
                                  ', '.join(str(p) for p in backups if p.exists()))
        for p, backup in zip(changed_paths, backups):
            shutil.copy2(p, backup)
        hp.write_text(txt)
        if geometry_changed:
            tp.write_text(tmx)
    print('schema %d:' % n); [print('  ', l) for l in log]


for n in parse_schemi():
    process(n)
print('(dry run: nessun file modificato)' if DRY else 'fatto: backup in *.bak')