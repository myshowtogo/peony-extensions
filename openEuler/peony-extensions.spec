%define debug_package %{nil}
Name:           peony-extensions
Version:        3.0.2
Release:        1
Summary:        Peony qt extensions (common files)
License:        LGPL-3.0+
URL:            http://www.ukui.org
Source0:        %{name}-%{version}.tar.gz

BuildRequires: cmake
BuildRequires: pkgconf
BuildRequires: glib2-devel 
BuildRequires: qt5-qtbase-devel
BuildRequires: qtchooser
BuildRequires: libpeony-dev
BuildRequires: peony
BuildRequires: poppler-qt5-devel
BuildRequires: gsettings-qt-devel

Requires: samba-common-tools
Requires: samba
Requires: engrampa
Requires: mate-terminal
Requires: libpeony3
Requires: qt5-qtimageformats
Requires: polkit

%description
 Peony is the official file manager for the UKUI desktop. This
 package adds extended functionality to the Peony file manager.

%prep
%setup -q

%build
export PATH=%{_qt5_bindir}:$PATH
mkdir qmake-build
pushd qmake-build
%{qmake_qt5}  ..
%{make_build}
popd

%install
rm -rf $RPM_BUILD_ROOT
pushd qmake-build
%{make_install} INSTALL_ROOT=%{buildroot}
popd

%clean
rm -rf $RPM_BUILD_ROOT

%post

%preun

%files
%doc debian/copyright  debian/changelog
%{_libdir}/peony-extensions
%{_prefix}/share/

%changelog
* Wed Feb 3 2021 lvhan <lvhan@kylinos.cn> - 3.0.2-1
- update to upstream version 3.0.2-1

* Tue Dec 15 2020 douyan <douyan@kylinos.cn> - 3.0.1-4
- remove post script

* Mon Dec 14 2020 douyan <douyan@kylinos.cn> - 3.0.1-3
- fix samba configure issue

* Tue Dec 8 2020 douyan <douyan@kylinos.cn> - 3.0.1-2
- fix-peony-share-failed-issue

* Wed Nov 11 2020 douyan <douyan@kylinos.cn> - 3.0.1-1
- update to upstream version 3.0.0-1+1031.1

* Mon Oct 26 2020 douyan <douyan@kylinos.cn> - 3.0.0-1
- update to upstream version 3.0.0

* Tue Sep 22 2020 douyan <douyan@kylinos.cn> - 2.1.2-2
- fix peony file share issue

* Thu Jul 09 2020 douyan <douyan@kylinos.cn> - 2.1.2-1
- Init package for openEuler
