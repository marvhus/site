# HTML markup inside MarkDown test

<div class="mydiv"> <!-- Can be used to section off text so that it can be styled differently -->
    Text inside .mydiv
</div>

<style>
    main .mydiv {
        padding: 1em;
        background-color: #EAEAEA;
    }
</style> <!-- Custom styling for said section -->

<link rel="stylesheet" href="/style.css"> <!-- Link to external CSS, specific to this one page... in this case it's pointless as the same stylesheet is linked in the head template -->

Text outside of custom markup

---

Iframe to different page
<iframe src="/index.html"></iframe>

---

<button onclick="alert('Stuff')">Button that does stuff</button>
