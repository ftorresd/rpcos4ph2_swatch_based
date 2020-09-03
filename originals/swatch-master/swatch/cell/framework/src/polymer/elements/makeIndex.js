#!/usr/bin/env node
var repositoryPath = "https://svnweb.cern.ch/trac/cactus/browser/trunk/cactuscore/swatchcell/framework/src/polymer/elements/";
var projectName = "SWATCH Cell";
var projectPath = "/swatchcell/framework/html/elements/"


var fs = require('fs');
var path = require('path');

function getDirectories(srcpath) {
  return fs.readdirSync(srcpath).filter(function(file) {
    return fs.statSync(path.join(srcpath, file)).isDirectory() && file.indexOf('-') > -1 && file.indexOf('template') == -1;
  });
}

var elements = getDirectories('.');
var result = [];
for (var i = 0; i < elements.length; i++) {
  var element = elements[i];
  var json = {name: element};
  if ( fs.existsSync(element + '/description.json') ) {
    var parsedJSON = require("./" + element + '/description.json');
    if (!parsedJSON.description) {
      console.error(element + '/description.json contains no description');
    }
    for (var property in parsedJSON) {
      if (parsedJSON.hasOwnProperty(property)) {
        json[property] = parsedJSON[property];
      }
    }
  } else {
    console.error(element + ' has no description.json file');
    json.description = "no description...";
  }
  if ( !fs.existsSync(element + '/index.html') ) {
    console.error(element + '/index.html does not exist');
  }
  result.push(json);
}

fs.readFile("./index_template.html", 'utf8', function(err, data) {
  if (err) {
    return console.error("index_template.html is missing or not readable");
  }
  data = data.replace(/<% data %>/g, JSON.stringify(result));
  data = data.replace(/<% repositoryPath %>/g, repositoryPath);
  data = data.replace(/<% projectName %>/g, projectName);
  data = data.replace(/<% projectPath %>/g, projectPath);
  fs.writeFile("index.html", data, function(err) {
      if(err) {
          return console.log(err);
      }

      console.log("index.html written");
  });
});
