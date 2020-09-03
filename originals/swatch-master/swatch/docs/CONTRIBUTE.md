/* CONTRIBUTE */

This is the contribute.md of our project. Great to have you here. 
Here are a few ways you can help make this project better!

# Contribute.md

## Team members

This section introduces the core team members. 

## Learn & listen

* Mailing list: hn-cms-L1TriggerSW@cern.ch 
* SVN repository: https://svnweb.cern.ch/trac/cactus
* Documentation: https://indico.cern.ch/event/285506/contribution/1/material/0/0.pdf 
* Vagrant information: https://github.com/cms-l1t-online/cactus-vagrant-box
* unit testing: http://alexott.net/en/cpp/CppTestingIntro.html

## Naming and Code style

Name conventions:
 * variables: camel case starting with lower case. First letter denotes the namespace ...
   - local variable = l
   - function parameter = a
   - constant global/static variable = k
   - non-constant global/static variable = s
   - class/struct private member variable = m
   - Exception: class/struct public member variable have no prefix
   - Examples: ```lLocalVariable, aParameterVariable, mClassMemberVariable```
 * methods: camel case starting with lower case. Example: ```getMeSomething();```
 * classes and enum types: camel case starting with upper case. Example: ```MyAwesomeFritter```
 * typedefs: Same as classes, but with `_t` suffix. Example: ```MyType_t```

Code style:
 * Indentation & braces: 
   - K&R (https://en.wikipedia.org/wiki/Indent_style#K.26R_style), as provided by most IDEs. Basic summary:
     - Functions: Opening brace on next line
     - Blocks within functions (if, while, for, ...): Opening brace on same line as control statement.
     - Namespaces: Opening brace on same line; no indentation due to namespace braces.
     - Closing braces: On line of their own, unless followed by ```else``` or ```while``` keywords
     - Statements within braces are indented, apart from namespace braces.
   - Initialisation list: Each member variable on its own line; one level of indentation.
   - Indent size: 2 spaces (NOT tab!)
 * Line wrap at 120 characters

Some of the above can be checked using the scripts in swatch/dev_tools/*. Please check ```<script> -h``` for usage instructions.

Unit tests:
 * prefer the use of ```BOOST_CHECK_EQUAL(<result expected to be false>, false)``` over ```BOOST_CHECK(!<result expected to be false>)``` for readability of failures.

## Adding new features
* This is how we like people to add new features:         
* Here are some specifics on the coding style we prefer:   
* This is how you send your pull request:                  
* You should include the following tests:                  
* These are the updates we hope you make to the changelog: 

# Bug triage
This sections explains how bug triaging is done for your project. Help beginners by including examples to good bug reports and providing them questions they should look to answer. 

* You can help report bugs by filing them here: https://svnweb.cern.ch/trac/cactus/newticket
* You can look through the existing bugs here: https://svnweb.cern.ch/trac/cactus/query?status=accepted&status=assigned&status=new&status=reopened&group=component&component=swatch&order=priority

* Look at existing bugs and help us understand if
** The bug is reproducible? Is it reproducible in other environments (browsers)? What are the steps to reproduce? 

* You can close fixed bugs by testing old tickets to see if they are
happening
* You can update our change log here
* You can remove duplicate bug reports


# Beta testing

This section explains if your project needs beta testing. Explain why early releases require heavy testing and how they can help with specially across browsers and on different hardware. 

* For our project you can find the roadmap and features that require
testing from here: 

# Translations

This section includes any instructions or translation needs your project has. 

* You can help us translate our project here: 

# Documentation

This section includes any help you need with the documentation and where it can be found. Code needs explanation, and sometimes those who know the code well have trouble explaining it to someone just getting into it. 

* Help us with documentation here

# Community 
This section includes ideas on how non-developers can help with the project. Here's a few examples:

* You can help us answer questions our users have here: 
* You can help build and design our website here:
* You can help write blog posts about the project by: 
* You can help with newsletters and internal communications by: 

* Create an example of the project in real world by building something or
showing what others have built. 
* Write about other people’s projects based on this project. Show how
it’s used in daily life. Take screenshots and make videos!


# Your first bugfix
This section should help a person get started with their very first bug fix and thinking through the problem.

* If you have further questions, contact: 


# git-svn for swatch (or any other cactus project)

## Getting started
 1) Get a minimal set of cactus modules to start with

```
# get the first level of files and directories
svn co --depth immediates  https://svn.cern.ch/reps/cactus/trunk cactus
cd cactus
```

 2) Get full content for scripts, config and cactuscore folders

```
svn update --set-depth=infinity  scripts config cactuscore
cd cactuscore/swatch
```

 3) create a git repo

```
git svn init svn+ssh://<your CERN username>@svn.cern.ch/reps/cactus/trunk/cactuscore/swatch
git svn fetch
```

 4) upload on github if you have an account
```
git remote add origin <your github repo url>
git push origin master
```

## Working on a ticket: via SVN

Any change that consists of more than one commit should be developed in a branch (rather than in the trunk).

### Step 1: Create the branch

The branch name should follow the following convention: developername_swatch/swatch_ticket_<ticketNumber>

To make a minimal branch containing the core SWATCH packages, plus the SWATCH cell, the SVN commands are:
```
svn mkdir -m "SWATCH, refs #ticketnumber : Creating branch" svn+ssh://svn.cern.ch/reps/cactus/branches/deveopername_swatch/swatch_ticket_ticketnumber
svn mkdir -m "SWATCH, refs #ticketnumber : Populating branch" svn+ssh://svn.cern.ch/reps/cactus/branches/deveopername_swatch/swatch_ticket_ticketnumber/cactuscore
svn cp -m "SWATCH, refs #ticketnumber : Populating branch" svn+ssh://svn.cern.ch/reps/cactus/trunk/cactuscore/swatch svn+ssh://svn.cern.ch/reps/cactus/branches/developername_swatch/swatch_ticket_ticketnumber/cactuscore/
svn cp -m "SWATCH, refs #ticketnumber : Populating branch" svn+ssh://svn.cern.ch/reps/cactus/trunk/cactuscore/swatchcell svn+ssh://svn.cern.ch/reps/cactus/branches/developername_swatch/swatch_ticket_ticketnumber/cactuscore/
svn cp -m "SWATCH, refs #ticketnumber : Populating branch" svn+ssh://svn.cern.ch/reps/cactus/trunk/config svn+ssh://svn.cern.ch/reps/cactus/branches/developername_swatch/swatch_ticket_ticketnumber/
```

### Step 2: Checkout code and commit to the branch

The magic command to check out the branch is:
```
svn co svn+ssh://svn.cern.ch/reps/cactus/branches/developername_swatch/swatch_ticket_ticketnumber
```

### Step 3: Merge back into the trunk

First, merge the latest trunk changes into the branch:
```
cd path/to/checked-out-branch/cactuscore/swatch
svn merge ^/trunk/cactuscore/swatch . 
# Rebuild & re-run unit tests
svn commit -m "SWATCH, refs #ticketnumber : Merging trunk changes into dev branch"
cd ../swatchcell
svn merge ^/trunk/cactuscore/swatchcell .
# Re-built & run tests as appropriate
svn commit -m "SWATCH, refs #ticketnumber : Merging trunk changes into dev branch"
```

Then, change directory into a clean checked-out copy (i.e. no local changes) of the trunk.
 * Run the SVN "merge" command with the --dry-run option, which will show the output of the command without really doing any changes into the repository. This is useful so that you make sure the merge only affects the files you expect it to affect.
```
cd cactuscore/swatch
svn merge --dry-run --reintegrate ^/branches/developername_my-project/my-project_ticketnumber/cactuscore/swatch
cd ../swatchcell
svn merge --dry-run --reintegrate ^/branches/developername_my-project/my-project_ticketnumber/cactuscore/swatchcell
```
 * Once you make sure the outcome of the dry run is as expected, run the same command without the --dry-run option, re-build, test, and commit if all goes well! I.e:
```
cd cactuscore/swatch
svn merge --reintegrate ^/branches/developername_my-project/my-project_ticketnumber/cactuscore/swatch
# Rebuild && run unit tests; if successful, continue ...
cd cactuscore/swatchcell
svn merge --reintegrate ^/branches/developername_my-project/my-project_ticketnumber/cactuscore/swatchcell
# Rebuild && run appropriate tests; if successful, continue ...
cd ../swatch
svn commit -m "SWATCH, refs #ticketnumber : Merge changes from dev branch into trunk"
cd ../swatchcell
svn commit -m "SWATCH, refs #ticketnumber : Merge changes from dev branch into trunk"
```

N.B. In case you get an error like:
```
svn: Reintegrate can only be used if revisions XXX through YYY were previously merged from svn+ssh://svn.cern.ch/reps/cactus/trunk/ZZZ to the reintegrate source, but this is not the case
```
You can fix it by going back into the branch directory and forcing SVN to merge the missing revisions before trying again to merge the branch into the trunk: 
```
svn merge -r XXX:YYY ^/trunk/ZZZ
```


## Working on a ticket: via git

### Create your git branch
First set the ticket number, your CERN user name and the branch name you will
be working on (assumes the ticket has been created)
```
export ticketnumber=<ticket number>
export cern_username=<CERN user name>
export branchname=${cern_username}_swatch_${ticketnumber}
```

Next create a git branch:

```
git checkout -b $branchname
```

Do your work.

### Create the SVN branch
Once everything is done and the tests are all passing create the SVN branch:

```
svn mkdir ^/branches/$branchname -m "Creating branch for ticket Refs #$ticketnumber"
svn cp -m "Creating branch for ticket Refs #$ticketnumber" ^/trunk/cactuscore/swatch ^/branches/$branchname

git config --add svn-remote.svn-${branchname}.url svn+ssh://$cern_username@svn.cern.ch/reps/cactus/branches/$branchname/swatch

git config --add svn-remote.svn-${branchname}.fetch :refs/remotes/svn-${branchname}
git svn fetch svn-${branchname} # it is faster if you add '-r <revision>' where revision == from branch creation
# git branch -a should now show your new git-svn branch
```

Create a local git branch based on the SVN branch

```
git checkout -b ${branchname}_merging remotes/svn-${branchname}
```

Since SVN has a separate history for branches you will need to rebase the
git branch, which has a complete history, onto the SVN branch:

```
git checkout $branchname
# where the commit has is the first commit you want to transfer
git rebase --onto ${branchname}_merging <commit-hash>^ 
git checkout ${branchname}_merging
git merge ${branchname}
# RUN TESTS!
# to check that the branch links are OK:
git svn dcommit --dry-run
# if everything is OK, commit:
git svn dcommit
```

### Reintegrating into the branch
Once you have the go to merge your branch with the trunk, you can do so by:

```
git checkout master
# you might need to update the SVN history:
svn revert -R .
svn update
# now let us try to reintegrate the changes
svn merge --reintegrate ^/branches/${branchname}/swatch --dry-run
# if everything looks OK, execute
svn merge --reintegrate ^/branches/${branchname}/swatch
svn commit -m "Merging ${branchname} for Refs #$ticketnumber" 
```

After a successful merge it is time to tidy up:

```
git branch -D -r svn-${branchname}
git branch -D ${branchname}
git branch -D ${branchname}_merging
rm -rf .git/svn/refs/remotes/svn-${branchname}
svn rm ^/branches/${branchname} -m "Deleting merged branch for Refs #$ticketnumber"
```
and make sure that your master is up-to-date (includes the merge)

```
git reset HEAD --hard
# remove all newly added files otherwise the next step will complain
git svn rebase
```

## FAQ
### Merging branches: Cannot merge into a working copy that has local modifications
In this case the svn cache is different to the git-svn one.
All you need to do is ```svn update```.
