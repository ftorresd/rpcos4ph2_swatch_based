module.exports = function(grunt) {
    grunt.initConfig({
        sass: {
            options: {
                sourcemap: 'none',
                style: 'expanded',
                noCache: true
            },
            dist: {
                files: [{
                    expand: true,
                    cwd: '',
                    src: [
                        'elements/**/*.scss'
                    ],
                    dest: '',
                    ext: '-min.css'
                }]
            }
        },
        postcss: {
            options: {
                map: false,
                processors: [
                    require('autoprefixer-core')({
                        browsers: ['firefox 24', 'IE 10', 'last 2 versions']
                    }),
                    require('cssnano')({
                        zindex: false,
                        mergeIdents: false,
                        reduceIdents: false,
                        discardUnused: false
                    })
                ]
            },
            dist: {
                src: ['elements/**/*-min.css']
            }
        },
        uglify: {
            options: {
                preserveComments: true,
                srewIE8: true,
                sourceMap: false,
                mangle: false
            },
            polymerjs: {
                files: [{
                    expand: true,
                    src: ['elements/**/*.js'],
                    ext: '-min.js'
                }]
            }
        },
        inline: {

          dist: {
            files: [{
              expand: true,
              cwd: '',
              src: [
                  'elements/**/*.html'
              ],
              dest: '../../html/',
              ext: '.html'
            }]
          }
      	},
        clean: {
            cssfiles: {
                options: {
                    'no-write': false
                },
                src: ["elements/**/*-min.css"]
            },
            jsfiles: {
                options: {
                    'no-write': false
                },
                src: ["elements/**/*-min.js"]
            },
            macjunk: {
                options: {
                    'no-write': false
                },
                src: ['**/._*', ".DS_Store", "**/.DS_Store"]
            }
        },
        execute: {
          target: {
            src: ['elements/makeIndex.js'],
            options: {
              cwd: "./elements/"
            }
          }
        }
    });

    grunt.loadNpmTasks('grunt-execute');
    grunt.loadNpmTasks('grunt-contrib-uglify');
    grunt.loadNpmTasks('grunt-postcss');
    grunt.loadNpmTasks('grunt-contrib-clean');
    grunt.loadNpmTasks('grunt-contrib-sass');
    grunt.loadNpmTasks('grunt-minify-html');
    grunt.loadNpmTasks('grunt-inline');

    grunt.registerTask('default', ['execute','sass', 'postcss', 'uglify', 'inline', 'clean']);
};
