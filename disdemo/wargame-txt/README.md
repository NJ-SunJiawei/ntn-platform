**CUI**

# wargame demo

This scenario is a demonstration of a simple wargame using Warlock. The scenario is best experienced with multiple players to take control of the four playable teams. The four teams, while of different colors, should have identical assets. Each player's team has available two fighters, a sam, a bomber, a sensor drone and a base.

The objective is for a player to use their assets to the best of their ability to destroy the opposing player's bases, while also defending their own. On start up, assets will follow predefined routes until directed otherwise.

Upon the loss of a base, a player will lose their remaining platforms in the 
scenario and receive a "Game Over" message. The last player with their base
in tact wins!

This directory contains six different start-up files:

* blue_team.txt - blue player startup
* green_team.txt - green player startup
* red_team.txt - red player startup
* purple_team.txt - purple player startup
* white_cell.txt - non-playable character for viewing the game
* single_player_scenario.txt - allows access and visibility of all teams

## Setup

### Command Line
1. open a command line (bash/powershell) in the demos directory (\path-to-afsim\demos\wargame)
1. Type the following command into the command line:  
   ` \your-path\warlock.exe -ifc  .\configs\<config-file> <start-up-file>`  
   Example:  
   `.\your-path\warlock.exe -ifc .\configs\RedConfig.ini .\red_team.txt`

### Warlock
1. Launch Warlock with one of the above start up txt 
   files found in this directory.
1. In Warlock click 'File' and select
   'Import Configuration Options'. (Import all settings)
1. Navigate to this directory using the file manager and open the 
   sub-directory configs.
1. Select your team's configuration.
1. Wait for all players to connect.
1. When all players are ready, click 'OK' in the 
   Ready Up prompt to begin the scenario.

**Note:** The single player start up file will allow vision of all 4 players
   and is available as a "sandbox mode" for convenient testing of custom modification.

For more information about this demo, please see the doc folder.

## CUI Designation Indicator
* Controlled by: Air Force Research Laboratory
* Controlled by: Aerospace Systems Directorate
* CUI Categories: CTI, EXPT
* LDC/Distribution Statement: DIST-C
* POC: afrl.rq.afsim@us.af.mil

## Notices and Warnings

### DISTRIBUTION STATEMENT C
Distribution authorized to US Government agencies and their contractors;
Critical Technology, Export Controlled; (2021 Mar 10). Other requests for this
information shall be referred to AFRL Aerospace Systems Directorate.

### NOTICE TO ACCOMPANY FOREIGN DISCLOSURE
This content is furnished on the condition that it will not be released to
another nation without specific authority of the Department of the Air Force of
the United States, that it will be used for military purposes only, that
individual or corporate rights originating in the information, whether patented
or not, will be respected, that the recipient will report promptly to the
United States any known or suspected compromise, and that the information will
be provided substantially the same degree of security afforded it by the
Department of Defense of the United States. Also, regardless of any other
markings on the document, it will not be downgraded or declassified without
written approval from the originating U.S. agency.

### WARNING - EXPORT CONTROLLED
This content contains technical data whose export is restricted by the Arms
Export Control Act (Title 22, U.S.C. Sec 2751 et seq.) or the Export
Administration Act of 1979, as amended, Title 50 U.S.C., App. 2401 et seq.
Violations of these export laws are subject to severe criminal penalties.
Disseminate in accordance with provisions of DoD Directive 5230.25.

### HANDLING AND DESTRUCTION NOTICE
Handle this information in accordance with DoDI 5200.48. Destroy by any
approved method that will prevent unauthorized disclosure or reconstruction of
this information in accordance with NIST SP 800-88 and 32 C.F.R 2002.14
(Safeguarding Controlled Unclassified Information).

**CUI**
