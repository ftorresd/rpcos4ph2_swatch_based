#!/usr/bin/env node
process.stdin.resume();
process.stdin.setEncoding('utf8');
var util = require('util');
var ncp = require('ncp').ncp;
var replace = require("replace");
var renamer = require("renamer");
var path = require('path');
ncp.limit = 16;
var FindFiles = require("node-find-files");
var fs = require('fs');
var exec = require('child_process').exec;

process.stdout.write('name of the new element: ');
process.stdin.on('data', function (text) {

  var split = text.replace('\n', '').split('/');
  if (split.length == 1) {
    base = split[0];
    newname = base;
  } else if (split.length == 2) {
    base = split[0];
    newname = split[1];
  } else {
    console.error('\nname can only contain only one dash (/)');
    process.exit();
  }

  if (newname == '') {
    console.error('\nname cannot be empty');
    process.stdout.write('name of the new element: ');

  } else if (newname.indexOf('-') == -1) {
    console.error('\nname must contain a dash (-)');
    process.stdout.write('name of the new element: ');

  } else {
    if (split.length == 1 ) {
      console.log('creating new element <' + base + '>...');
    } else {
      console.log('creating new element <' + newname + '> in <' + base + '>...');
      return console.error("unfortunately we can't do this because we will mess up the .svn folders");
    }

    ncp('template-element', base, function (err) {
      if (err) { return console.error(err); }
      replace({
        regex: "template-element",
        replacement: newname,
        paths: [base],
        recursive: true,
        silent: true,
      });


      var finder = new FindFiles({
        rootFolder : base,
        filterFunction : function (path, stat) {
          return (path.indexOf('template-element') > -1) ? true : false;
        }
      });
      finder.on("match", function(strPath, stat) {
        // console.log(strPath + " -> " + strPath.replace('template-element', newname));
        fs.rename(strPath, strPath.replace('template-element', newname), function(err) {
          if ( err ) console.log('ERROR: ' + err);
        });
      })
      finder.on("complete", function() {
        console.log("removing any .svn folders in ", newname);
        exec('rm -rf `find ' + newname + ' -type d -name .svn`', function (err, stdout, stderr) {});
        console.log("Finished");
        process.exit();
      })
      finder.on("patherror", function(err, strPath) {
        // Note that an error in accessing a particular file does not stop the whole show
        console.log("Error for Path " + strPath + " " + err);
      })
      finder.on("error", function(err) {
        console.log("Global Error " + err);
        process.exit();
      })
      finder.startSearch();
    });
  }

});
