'use strict';

console.log("BEGIN");    


const fs = require("fs");

let filename = "test.txt";

console.log(`Reading ${filename}`);

fs.readFile(filename, 'utf-8', function(err, data) {
    if (err) throw err;
    console.log('OK: ' + filename);
    console.log(data)
});

console.log("END");

