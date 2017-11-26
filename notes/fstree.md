
# file system structure

**IN THE CONTENT DIRECTORY:** `.yabakignore`

`config`: like git *.git/config* file, in INI format

## blobs/

`<01>/<23>/<45...??>.blob`(length: 2/2/60 .blob): xz-compressed up-to-8MB blob, whose *SHA-256* is *"012345...??"*

## meta/
`<snapshot_hash % 997: 000-996>.json`

````json
[
    {
        "id": "<commit SHA256 hash>",
        "user": "<committer name>",
        "time": "<UTC time>",
        "message": "<commit message>"
    }, {
        ...  // similar to previous one
    }
]
````

## snapshots/

#### snapshots/<01>/<23>/<45...??>/ *(this is one commit)*

`meta.json`: id, user, time, message, ...

`00000001.json, 00000002.json, ...`: compressed files / directories.
dynamic splitting according to json file size

````json
[
    {
        "id": "<file/directory hash>",
        "blobs": [ "<256bit>", "<256bit>" ]
]
````
