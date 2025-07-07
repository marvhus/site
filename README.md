# Site

Template for building simple static websites from markdown files.

## How to build static HTML files

The markdown files are in `pages/`.  To convert these to HTML, build the `builder` tool found in `tools/`.
(You can do this using the Makefile in the root of the repository, just run `make` while in the root of the repository.)
The builder tool binary will be placed in `tools-bin`.  Just run this binary while in the root of the repository to convert the
markdown files into HTML.  These will be placed in `public/` following the same structure as in the `pages/` directory.

The templates used to build the HTML files are located in `templates/`

Any hardcoded files, like any CSS you may want is located in `web/`.  There currently is no tooling to automatically copy over
this to the `public/` folder so you have to do this yourself.
