#!/bin/sh
echo "#!/usr/bin/tclsh" > minimosd_cfg
cat mavlink.tcl mavlink_osd.tcl ihex.tcl minimosd_cfg.tcl >> minimosd_cfg
chmod +x minimosd_cfg
