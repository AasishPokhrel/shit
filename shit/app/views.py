from django.http import HttpRequest, HttpResponse


def shit(_: HttpRequest) -> HttpResponse:
    return HttpResponse('Shit')
