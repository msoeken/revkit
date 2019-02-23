venv: venv/bin/activate
venv/bin/activate: requirements.txt docs/requirements.txt
	test -d venv || virtualenv venv
	venv/bin/pip install -r requirements.txt
	venv/bin/pip install -r docs/requirements.txt
	venv/bin/pip install qiskit==0.7.1
	touch venv/bin/activate

devbuild: venv
	venv/bin/python setup.py install

docs: devbuild docs/*.rst docs/conf.py
	PATH=$(PWD)/venv/bin/:$(PATH) $(MAKE) -C docs html

test: devbuild
	venv/bin/python setup.py test
