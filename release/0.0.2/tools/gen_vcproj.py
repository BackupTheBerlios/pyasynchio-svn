from sys import argv
from pywintypes import CreateGuid
from string import replace

template_file_name = argv[1]
project_name = argv[2]

template = file(template_file_name).read()
output = replace(template, "%ProjectName%", project_name)
output = replace(output, "%ProjectGUID%", str(CreateGuid()))
print output