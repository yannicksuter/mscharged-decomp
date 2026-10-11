# GitHub Actions

The [build workflow](../.github/workflows/build.yml) supports `R4QE01`,
`R4QP01_rev1` and `R4QP01_rev2`. It uses a private container for original
executables and the compiler packages.

Supply each original as `/orig/<version>/sys/main.dol` in that container.
Keep game files out of Git, including the private build repository: provide
them through a private build context or private storage instead. The public
workflow uploads only reconstructed linker maps and progress reports.

USA remains required. EU jobs report a notice and skip their build until the
corresponding original is available in the container. Once supplied, each job
builds all sources and checks its revision's retail DOL hash. Local builds do
not require a container.

- [Build Repository](#build-repository)
- [Workflow](#workflow)
- [decomp.dev](#decompdev)

## Build Repository

This repository will be used to build and store the CI build container.

> [!CAUTION]
> This repository should be **private** to avoid exposing the game's assets.

1. [Create a **private** repository from `encounter/dtk-template-build`](https://github.com/new?template_name=dtk-template-build&template_owner=encounter). A common name is your project's repository name with `-build` appended. For example, `tww-build`.

2. Supply `orig/<version>/sys/main.dol` through the private container build
   context, using the version names above. **Do not commit these files.**
   Only the original executable is needed for this project.

3. Once the build container action completes, visit the package settings:  
    ![GitHub repository packages](images/github_build_repo_packages.png)  
    ![GitHub package settings](images/github_package_settings.png)

4. Under "Manage Actions access", add your project's main repository with the "Read" role:  
    ![GitHub package Actions access](images/github_package_settings_access.png)

## Workflow

1. In `.github/workflows/build.yml`, set `container:` to the private
   [build image](#build-repository).

2. Make all three original executables available in that image under their
   separate version directories.

The workflow builds each available version on every push or pull request.

## decomp.dev

Once the build workflow is running on the main branch, you can add your game to <https://decomp.dev>.

Visit <https://decomp.dev/manage/new>, select your GitHub repository and fill out the required fields.

If you have questions or issues, try asking in the [GC/Wii Decompilation Discord](https://discord.gg/hKx3FJJgrV) #decomp.dev channel.
