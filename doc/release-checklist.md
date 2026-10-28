Checklist of things to do before releasing a new version.

- Make sure that the Copyright year in '/src/scene/prestart.c' is up to
  date.
- Change the version number in '/src/scene/start.c'.
- Change the 'Software Version' and 'Complement Check' in 'header.s';
  then, make sure the ROM is valid and that the GBA BIOS accepts it.
- Update the changelog file.

Expanded Edition (fork) additions:

- Version: "EE Vx.y" in '/src/scene/start.c', 'Software Version' in
  'header.s' (0 = 1.0, 1 = 1.1, ...). The header checksum must change by
  the same amount in the opposite direction; check it with
  `python tools/check-header.py bin/minicraft-expanded-edition.gba`.
- If the save layout changed, bump SAVE_VERSION in '/src/storage.c' and
  say so in the changelog.
- After art changes: run 'tools/art-source/update_art.py' and
  'tools/art-source/make_previews.py'.
- Build both ROMs (normal and `DEBUG_KIT=1`), run `make test`, and play
  the debug ROM through the test rooms.
- Update the numbers in CLAUDE.md (ROM, IWRAM, EWRAM, stack).
- If the look changed, refresh the README screenshots:
  `python tools/test/screenshots.py` (needs the debug ROM).
- Tag the release and push the tag (`git tag ee-vx.y`,
  `git push origin ee-vx.y`): the Build workflow then creates the GitHub
  release with both ROMs and the newest CHANGELOG.md section as notes.
