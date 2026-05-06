# TRGR

<cr>THIS MOD IS IN ALPHA! BUGS AND CRASHES MAY HAPPEN SO BACKUP ANY LEVEL YOU USE THIS ON!</c>

## General

TRGR is an extension to the GD-Editor that aims to provide a way of automating certain actions using scripts. By using lua, you can create scripts that change things in the opened level. More on making your own script will be explained later.

To use any script open the editor, pause and click the TRGR-Button at the bottom of the screen. Then hit the Import-button and select the `.json`-File.
By clicking the Run-Button on any Script it will execute. Sometimes you will need to Save and Quit and reload the level after running a script. Also be careful with scripts from an untrusted source as they could do unintended things to your level such as messing up or destroying your entire level.

In case the Script-Files need to be accessed navigate to `C:\Users\[USER]\AppData\Local\GeometryDash\geode\mods\fabbeyond.trgr\`

---

## Script Creation

TRGR uses lua as it's base with more features, specifically for the GD-Editor. \
To start, create a file with the `.lua`-Extension and open it in any text editor (Notepad, Visual Studio Code, ...). \
Then using lua syntax and the added functions, create your own script. \
More on the added functions later.

After you're done coding, head into the editor, open the TRGR-Popup from the Pause-Screen and hit the Create-Button.
Enter your scripts name and description and pick your `.lua`-File.
Then hit Create and your new script will be added to the scripts.

To update a script, simply type in the same name and description and pick the updated `.lua`-File.
Therefore it will overwrite any script with the same name* when you create one.

\* Same name refers to the same name after your turn all characters to lowercase and replace any space with an underscore. \
For example: \
`Cool Script` -> `cool_script` \
`My first Script` -> `my_first_script`

## Example scripts

Move every selected block up by 2 blocks
```
-- Get every selected block
local blocks = level.getSelected()

-- Loop through them
for _, obj in ipairs(blocks) do

    -- Move the object up by 2 blocks
    -- 1 block = 30 steps
    obj:move(60, 0)
end
```

Delete every selected object below y 300
```
-- Get every selected block
local blocks = level.getSelected()

-- Loop through them
for _, obj in ipairs(blocks) do

    -- Check if the block is below y 300
    if obj.y < 300 then

        -- Delete object
        obj:delete()
    end
end

```

---

## New Functions

TRGR adds a class called `level`. \
All supported methods of `level`:
- `.getByID(id)`
- `.getAll()`
- `.getSelected()`
- `.getByGroup(group)`
- `.log(message)` \
\
Note: log currently only supports `string` \
\
TRGR also adds a type: `GameObject`. \
This type has the following properties:
- `.id`
- `.x`, `.y`
- `.scaleX`, `.scaleY`
- `.rotation`
- `.zOrder`
- `.zLayer`
- `.flipX`, `.flipY`\
\
The `GameObject` type also has the following methods:
- `:move(x, y)`
- `:moveTo(x, y)`
- `:rotate(degrees)`
- `:scale(x, y)`
- `:delete()`
- `:addGroup(group)`
- `:removeGroup(group)`