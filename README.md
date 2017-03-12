# ObDump
Raw dump of the Windows Object Manager (like Winobj but in cmd)

# Usage
Just compile and run it in your machine.

The output format should be:
"%(ObjectPath)s,%(ObjectType)s"
"%(ObjectPath)s,SymbolicLink->%(SymlinkTarget)s"
...

# Credit
All credit goes to Pavel Yosifovich:
Blog post: http://blogs.microsoft.co.il/pavely/2014/02/05/creating-a-winobj-like-tool/
Original code: https://bitbucket.org/zodiacon/objectmanagerbrowser
