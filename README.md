This repository has been archived. A recent version can be found on https://gitlab.com/project-march.

# 🤖 Ethercat Slaves
Repository for the General EtherCAT Slaves (GES) and the PDB EtherCAT Slave of the March exoskeleton.

The DieBieSlave is used as the GES in the MARCH IV. The hardware of the PDB EtherCAT Slave is developed by MARCH IV.

| Branch | Build Status |
| ------ |:------------:|
| master | [![Build Status](https://api.travis-ci.com/project-march/ethercat-slaves.svg?branch=master)](https://travis-ci.com/project-march/ethercat-slaves) |
| develop | [![Build Status](https://api.travis-ci.com/project-march/ethercat-slaves.svg?branch=develop)](https://travis-ci.com/project-march/ethercat-slaves) |


## 🛠️ Building
This project uses the cross platform builder [platformio](https://docs.platformio.org/en/latest/what-is-platformio.html).
So you will need to install that first.

The project contains multiple [environments](https://docs.platformio.org/en/latest/projectconf/section_env.html#projectconf-section-env) for each slave. The environments you can choose are:

* `bp_ges`: Backpack GES
* `lll_ges`: Left lower leg GES
* `lul_ges`: Left upper leg GES
* `rll_ges`: Right lower leg GES
* `rul_ges`: Right upper leg GES
* `pdb`: Power distribution board slave
* `standalone_pdb`: Power distribution board, which does not use ethercat

To build all of the projects you run:

    $ pio run

This will build all the projects sequentially. When you want to build one specific environment only you can use the `--environment` (`-e`) option:

    $ pio run -e <slave>

So, for example, to build the backpack GES you run `pio run -e bp_ges`.
To actually upload the project, connect the the GES you would like to upload to and use the `--target` (`-t`) option.

    $ pio run -e <slave> -t upload
