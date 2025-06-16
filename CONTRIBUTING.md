# Contributing to SuperTux

## Bug reports

- GitHub Issues in English only. IRC support might be possible in other languages,
  however English is generally preferred so everyone can understand what's happening.
- Please check for duplicate issues on GitHub, as they're wasting the developers'
  time.
- Check if your issue has been resolved in the latest development version.
- Separate issues and feature requests. Don't put all in the same issue.
- If you made a mistake in your bug report, please use GitHub's edit feature
  to correct the wrong detail, instead of closing the old issue and opening a
  new one.


### Required information

Please include the following details in your issue report so that we can investigate:

- The SuperTux version you're using, as printed in the bottom left corner of the
  menu or in `supertux2 -v`.
- Details about your kernel, operating system and distribution, including name
  and version details.
- If possible, provide steps to reproduce your issue.
- If available, it's a great help for the developers to include a backtrace from
  `gdb` or a core dump. If you're on GNU/Linux and your distro is using systemd, it's
  highly likely that a core dump is available using `coredumpctl(1)`.

### Add-on related issues

Issues regarding add-ons found in the official SuperTux Add-on repository
should be filed at [SuperTux/addons](https://github.com/SuperTux/addons/issues).

No support is provided for third-party add-ons (that doesn't include official
add-ons or language packs). However, a pull request against the [SuperTux/addons](https://github.com/SuperTux/addons)
repository, containing a fix, is always welcome!

### Translation issues

Please use [Transifex](https://explore.transifex.com/arctic-games/supertux/) to fix or report translation related issues.

## Pull requests

- No duplicate pull requests, please.
- Please test your code to see if it works correctly. If the pull request
  is untested or work-in-progress, please mention that.
- Pull requests importing translation updates from Transifex are, unless requested
  by a contributor, not very welcome.
- Commit messages should follow this style:
  - First line: Short summary, up to 72 characters
  - An empty line, separating message subject and body
  - Detailed explanatory text for the change which can include multiple paragraphs,
    or bullet points. Maximum of 72 characters per line.
  - If the commit fixes an issue reported on GitHub, please add another empty
    line, followed by a line containing `Closes #xyz`, where `#xyz` is the issue
    to close. See <https://help.github.com/articles/closing-issues-via-commit-messages/>
    for a more detailed description of this feature.
  - If the commit doesn't include any changes that need CI testing (code or
    build system changes should always be CI tested, other things not), yet another
    empty line, followed by a line containing `[ci skip]`. This makes the CI
    ignore that commit.
- Please [squash](https://help.github.com/articles/about-git-rebase/) commits that belong together.
  If you forget initially, it's okay to force push to your own fork to fix it.
- No merge commits inside pull requests. Use `git-rebase` to keep your branch
  up-to-date.
- To make it easier for everyone, don't make a pull request pulling your fork's
  `master` branch.
