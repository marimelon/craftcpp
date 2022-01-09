FROM gitpod/workspace-full

USER gitpod

RUN pyenv install 3.9.2 && pyenv local 3.9.2 && pip install pytest pybind11-stubgen