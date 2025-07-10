# Site

Template for building simple static websites from markdown files.

## Compiling the purpose built tools

Just run the `make` command to compile all the tools found in `tools/`.
The binaries will be placed in `tools-bin/`.

## Changing the templates used when building the HTML pages

The templates are placed in `templates/`.
There you will find some files that are used as the HTML around the content of each page.
This is used so you don't need to manually update every page when you want to update the head, navbar, or footer.
Updating any of these files and using the `builder` tool again will update every page to contain these changes.

## Building the markdown pages into HTML

You can use the `builder` tool found in `tools-bin/` to convert the files in `pages/` to HTML files placed into `public/`.
Just run the binary while in the root of the project.

## Including extra content

To include content not created by the `builder` tool, place it in `web/`.
In the current example there is a CSS file, which is the style for the website.
You can use it for anything you want, for example you could place the images you want to show on the website here, or you could include HTML you have written from scratch in case you want full control over the layout of a specific page, etc.

## Deploying

To deploy, just serve the content in the `public/` directory.
