This folder contains everything linked to the server that will aggregate the
data from our chips.

## How to start flask
You can start flask privately (as in, it will not be visible on the network, 
meaning the chip won't be able to send data to it) with:

```sh
flask --app main.py run
```

To see errors and changes in live, you can enable debug mode (not safe !) with:

```sh
flask --app main.py run --debug
```
