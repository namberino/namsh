# TTY modes

Unix-like TTY drivers has 2 modes: *cooked* and *raw*

In cooked mode, there's command-line editing, which means the user can type their input, edit their input (deleting, retyping, etc) and the program won't see it until the user hits enter. In raw mode, the TTY driver passes every character to the program that it is typed

By default, programs on Unix-like systems starts in cooked mode by default and if we want to use raw mode, we have to manually enable it

Raw mode is necessary for our shell as we need to handle keys in a different way. The `tab` key needs to put 4 spaces, the `backspace` needs to be able to delete tabs, the `tab` key needs to also handle autocompletion as well, arrow keys are a whole nother story, etc. So we need to enable raw mode whenever we input something
