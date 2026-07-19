const API_KEY = "He648rg8WG156IqVlg3XIHCtayG1MbU1TtiX2szRliWjeUzccw";
const BLOG_ID = "mcupdate.tumblr.com";

const URL_PARAMETERS = new URLSearchParams(window.location.search);
const GLOBAL_POST_CURRENT = parseInt(URL_PARAMETERS.get('post')) || -1;
const GLOBAL_PAGE_CURRENT = parseInt(URL_PARAMETERS.get('page')) || 1;

const ViewMode = {
    Pages: "Pages",
    Posts: "Posts"
};

// This is atrocious
const internalEntries = [
    "26-2.json",                        // 16/6/26
    "26-1-2.json",                      // 9/4/26
    "26-1-1.json",                      // 1/4/26
    "26-1.json",                        // 24/3/26
    "1-21-11.json",                     // 9/12/25
    "1-21-10.json",                     // 7/10/25
    "1-21-9.json",                      // 30/9/25
    "1-21-8.json",                      // 17/7/25
    "1-21-7.json",                      // 30/6/25
    "1-21-6.json",                      // 17/6/25
    "1-21-5.json",                      // 25/3/25
    "1-21-4.json",                      // 3/12/24
    "1-21-3.json",                      // 23/10/24
    "1-21-2.json",                      // 22/10/24
    "1-21-1.json",                      // 8/8/24
    "1-21.json",                        // 13/6/24
    "1-20-6.json",                      // 29/4/24
    "1-20-5.json",                      // 23/4/24
    "1-20-4.json",                      // 7/12/23
    "1-20-3.json",                      // 5/12/23
    "1-20-2.json",                      // 21/9/23
    "1-20-1.json",                      // 12/6/23
    "1-20.json",                        // 7/6/23
    "1-19-4.json",                      // 14/3/23
    "1-19-3.json",                      // 7/12/22
    "1-19-2.json",                      // 5/8/22
    "1-19-1.json",                      // 27/7/22
    "1-19.json",                        // 7/6/22
    "1-18-2.json",                      // 28/2/22
    "1-18-1.json",                      // 9/12/21
    "1-18.json",                        // 30/11/21
    "1-17-1.json",                      // 6/7/21
    "1-17.json",                        // 7/6/21
    "1-16-5.json",                      // 14/1/21
    "1-16-4.json",                      // 3/11/20
    "1-16-3.json",                      // 10/9/20
    "announcing-minecraft-live.json",   // 3/9/20
    "1-16-2.json",                      // 11/8/20
    "1-16-1.json",                      // 24/6/20
    "1-16.json",                        // 23/6/20
    "1-15-2.json",                      // 21/1/20
    "1-15-1.json",                      // 17/12/19
    "1-15.json",                        // 10/12/19
    "1-14-4.json",                      // 19/7/19
    "1-14-3.json",                      // 24/6/19
    "1-14-2.json",                      // 27/5/19
    "1-14-1.json",                      // 13/5/19
    "1-14.json",                        // 22/4/19
    "1-13-2.json",                      // 22/10/18
    "1-13-1.json",                      // 22/8/18
    "1-13.json",                        // 22/7/18
    "1-12-2.json",                      // 18/9/17
    "1-12-1.json",                      // 3/8/17
    "1-12.json"                         // 7/6/17
];

// To prefer Posts to Pages
const CURRENT_VIEWMODE = GLOBAL_POST_CURRENT > -1 ? ViewMode.Posts : ViewMode.Pages;

const GLOBAL_FETCH_LIMIT = 10;

// we can safely assume 72 as it hasn't been updated since 2016/2020
const TUMBLR_POST_TOTAL = 72;
const INTERNAL_POST_TOTAL = internalEntries.length;
const GLOBAL_POST_TOTAL = INTERNAL_POST_TOTAL + TUMBLR_POST_TOTAL;

const GLOBAL_FETCH_OFFSET = (GLOBAL_PAGE_CURRENT - 1) * GLOBAL_FETCH_LIMIT;
const TUMBLR_FETCH_OFFSET = GLOBAL_FETCH_OFFSET - INTERNAL_POST_TOTAL;

const POSTS = document.getElementById("posts");

let INTERNAL_POST_LOADED = 0;
let GLOBAL_POST_ID = 1 + GLOBAL_FETCH_OFFSET;

async function fetchFromInternal(limit = GLOBAL_FETCH_LIMIT, offset = 0){
    const baseUrl = "./internalEntries/";

    let entries = [];

    for (let i = offset; i < internalEntries.length && i < offset + limit; i++) {
        let file = internalEntries[i];
        const url = `${baseUrl}/${file}`;

        try {
            const response = await fetch(url);

            if (!response.ok) {
                throw new Error(`Response status: ${response.status}`);
            }

            const result = await response.json();

            INTERNAL_POST_LOADED++;
            entries.push(result);
        }

        catch (error) {
            console.error(error.message);
        }
    }


    await createLogEntries(entries);

}
async function fetchFromTumblr(limit = GLOBAL_FETCH_LIMIT, offset = 0) {
    if (offset + limit < 0) {
        return;
    }

    const url = `https://api.tumblr.com/v2/blog/${BLOG_ID}/posts?api_key=${API_KEY}&limit=${limit}&offset=${offset}`;

    console.log(`Current post: ${GLOBAL_POST_CURRENT}`);

    try {
        const response = await fetch(url);
        if (!response.ok) {
          throw new Error(`Response status: ${response.status}`);
        }

        const result = await response.json();

        if (!(result.meta && result.meta.status === 200)) {
            console.error("Tumblr API Error:", result.meta ? result.meta.msg : "Unknown Error");
        }

        console.log(result);

        await createLogEntries(result.response.posts);
    }
    catch (error) {
        console.error(error.message);
    }
}

async function createLogEntries(entries){
    POSTS.appendChild(createSeparator());

    // Early return for post soloing
    if (GLOBAL_POST_CURRENT !== -1) {
        POSTS.appendChild(createSeparator());
        POSTS.appendChild(await parsePost(entries[0], GLOBAL_POST_CURRENT));
        return;
    }

    for (const entry of entries){
        POSTS.appendChild(createSeparator());
        POSTS.appendChild(await parsePost(entry));
        GLOBAL_POST_ID++;
    }
}

async function parsePost(post) {
        const item = document.createElement("div");
        item.className = "post text";

        const title = document.createElement("h3");
        const titleLink = document.createElement("a");

        titleLink.textContent = post.title;
        titleLink.href = `?post=${GLOBAL_POST_ID}`;

        title.appendChild(titleLink);

        const body = document.createElement("p");

        body.innerHTML = post.body;

        item.appendChild(title);
        item.appendChild(body);

        return item;
}

function createSeparator() {
    const separator = document.createElement("div");
    separator.style.width = "100%";
    separator.style.height = "16px";
    return separator;
}

function createFooter() {
    const footer = document.createElement("div");
    footer.id = "footer";

    const pagination = document.createElement("div");
    pagination.id = "pagination";

    if ((CURRENT_VIEWMODE === ViewMode.Pages && GLOBAL_PAGE_CURRENT > 1) || (CURRENT_VIEWMODE === ViewMode.Posts && GLOBAL_POST_CURRENT > 1)) {
        const newer = document.createElement("a");
        newer.id = "newer";
        newer.title = "Newer Posts";
        newer.innerHTML = "Newer";

        if (CURRENT_VIEWMODE === ViewMode.Posts) {
            newer.href = `?post=${GLOBAL_POST_CURRENT - 1}`;
            console.log("Adding Newer Footer Button for Posts!");
        }
        else if (CURRENT_VIEWMODE === ViewMode.Pages) {
            newer.href = `?page=${GLOBAL_PAGE_CURRENT - 1}`;
            console.log("Adding Newer Footer Button for Pages!");
        }
        pagination.appendChild(newer);
    }

    if ((CURRENT_VIEWMODE === ViewMode.Pages && (GLOBAL_FETCH_OFFSET + GLOBAL_FETCH_LIMIT) < GLOBAL_POST_TOTAL) || (CURRENT_VIEWMODE === ViewMode.Posts && GLOBAL_POST_CURRENT < GLOBAL_POST_TOTAL)) {

        const older = document.createElement("a");
        older.id = "older";
        older.title = "Older Posts";
        older.innerHTML = "Older";

        if (CURRENT_VIEWMODE === ViewMode.Posts) {
            older.href = `?post=${GLOBAL_POST_CURRENT + 1}`;
            console.log("Adding Older Footer Button for Posts!");
        }
        else if (CURRENT_VIEWMODE === ViewMode.Pages) {
            older.href = `?page=${GLOBAL_PAGE_CURRENT + 1}`;
            console.log("Adding Older Footer Button for Pages!");
        }
        pagination.appendChild(older);
    }

    console.log("Appending footer!");
    footer.appendChild(pagination);
    POSTS.appendChild(footer);
}

if (CURRENT_VIEWMODE === ViewMode.Posts) {
    // Get soloed post
    if (GLOBAL_POST_CURRENT > INTERNAL_POST_TOTAL) {
        fetchFromTumblr(1, GLOBAL_POST_CURRENT - INTERNAL_POST_TOTAL - 1).then(r => createFooter());
    }
    else {
        fetchFromInternal(1, GLOBAL_POST_CURRENT - 1).then(r => createFooter());
    }
}
else {
    fetchFromInternal(GLOBAL_FETCH_LIMIT, GLOBAL_FETCH_OFFSET).then(r => {
    console.log(`Total Internal Posts: ${INTERNAL_POST_TOTAL}`);
    console.log(`Tumblr Posts to be loaded: ${GLOBAL_FETCH_LIMIT - INTERNAL_POST_LOADED}`);

    fetchFromTumblr(GLOBAL_FETCH_LIMIT - INTERNAL_POST_LOADED, TUMBLR_FETCH_OFFSET).then(r => createFooter());
    });
}

